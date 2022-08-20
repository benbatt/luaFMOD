/*
Copyright 2022 Ben Batt

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#include "callbacks.h"
#include "common.h"
#include "platform.h"

static LUAFMOD_CRITICAL_SECTION *sCriticalSection = NULL;
static lua_State *sCallbackState = NULL;

static FMOD_RESULT affirmCriticalSection()
{
    if (!sCriticalSection) {
        sCriticalSection = criticalSectionCreate();

        if (!sCriticalSection) {
            return FMOD_ERR_MEMORY;
        }
    }

    return FMOD_OK;
}

extern int LUAFMOD_EXPORT luaopen_luaFMOD(lua_State *L);

static FMOD_RESULT affirmCallbackState(lua_State *L)
{
    if (!sCallbackState) {
        void *userdata = NULL;
        lua_Alloc alloc = lua_getallocf(L, &userdata);

        sCallbackState = lua_newstate(alloc, userdata);

        if (!sCallbackState) {
            return FMOD_ERR_MEMORY;
        }

        luaL_openlibs(sCallbackState);
        luaopen_luaFMOD(sCallbackState);
    }

    return FMOD_OK;
}

#define CALLBACK_TABLE "luaFMOD_Callbacks"
#define USERDATA_TABLE "luaFMOD_Userdata"

static void affirmCallbacksTable()
{
    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);

    if (lua_isnil(sCallbackState, -1)) {
        lua_newtable(sCallbackState);
        lua_setfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    }

    lua_pop(sCallbackState, 1);
}

static void affirmUserdataTable()
{
    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, USERDATA_TABLE);

    if (lua_isnil(sCallbackState, -1)) {
        lua_newtable(sCallbackState);
        lua_setfield(sCallbackState, LUA_REGISTRYINDEX, USERDATA_TABLE);
    }

    lua_pop(sCallbackState, 1);
}

#define CHUNK_CAPACITY_DEFAULT 128

typedef struct Chunk {
    struct Chunk *next;
    size_t size;
    size_t capacity;
} Chunk;

typedef struct ChunkList {
    Chunk *first;
    Chunk *last;
} ChunkList;

static int chunkListWrite(lua_State *L, const void *p, size_t size, void *userdata)
{
    ChunkList *list = (ChunkList*)userdata;

    Chunk *chunk = list->last;

    if (!chunk || (chunk->size + size) > chunk->capacity) {
        size_t capacity = max(CHUNK_CAPACITY_DEFAULT, size);

        chunk = malloc(sizeof(*chunk) + capacity);

        if (!chunk) {
            return 1;
        }

        chunk->size = 0;
        chunk->capacity = capacity;
        chunk->next = NULL;

        if (list->last) {
            list->last->next = chunk;
        } else {
            list->first = chunk;
        }

        list->last = chunk;
    }

    char *buffer = (char*)(chunk + 1);
    memcpy(buffer + chunk->size, p, size);

    chunk->size += size;

    return 0;
}

static const char *chunkListRead(lua_State *L, void *userdata, size_t *size)
{
    Chunk **iterator = (Chunk**)userdata;
    Chunk *chunk = *iterator;

    if (chunk) {
        *iterator = chunk->next;

        *size = chunk->size;
        return (char*)(chunk + 1);
    } else {
        *size = 0;
        return NULL;
    }
}

static int chunkListLoad(lua_State *L, ChunkList *chunkList, const char *chunkName)
{
    Chunk *currentChunk = chunkList->first;

    return lua_load(L, chunkListRead, &currentChunk, chunkName);
}

static void chunkListFree(ChunkList *chunkList)
{
    for (Chunk *current = chunkList->first; current;)
    {
        Chunk *previous = current;

        current = current->next;

        free(previous);
    }

    chunkList->first = NULL;
    chunkList->last = NULL;
}

FMOD_RESULT F_CALLBACK eventCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event,
    void *parameters)
{
    criticalSectionEnter(sCriticalSection);

    lua_State *L = sCallbackState;

    lua_getfield(L, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    lua_pushlightuserdata(L, event);
    lua_gettable(L, -2);

    if (!lua_isnil(L, -1)) {
        CONSTANT_access_FMOD_STUDIO_EVENT_CALLBACK_TYPE(L, &type, 0, 0);
        CREATE_USERDATA(FMOD_STUDIO_EVENTINSTANCE, event);

        if (lua_pcall(L, 2, 0, 0) != 0) {
            const char *error = lua_tostring(L, -1);
            int breakHere = 1;
        }
    }

    criticalSectionLeave(sCriticalSection);

    return FMOD_OK;
}

int callbackPrepare(lua_State *L, int index, void *owner)
{
    luaL_checktype(L, index, LUA_TFUNCTION);

    const char *upvalue = lua_getupvalue(L, index, 1);

    if (upvalue) {
        return luaL_error(L, "Attempt to set a callback with at least one upvalue (%s)", upvalue);
    }

    REQUIRE_OK(affirmCriticalSection());

    criticalSectionEnter(sCriticalSection);

#define CHECK_RESULT(r) \
    do { \
        FMOD_RESULT _r = (r); \
        if (_r != FMOD_OK) { \
            criticalSectionLeave(sCriticalSection); \
            REQUIRE_OK(_r); \
        } \
    } while (0)

    CHECK_RESULT(affirmCallbackState(L));

    affirmCallbacksTable();

    lua_pushvalue(L, index);

    ChunkList chunkList = { 0 };

    if (lua_dump(L, chunkListWrite, &chunkList) != 0) {
        criticalSectionLeave(sCriticalSection);
        return luaL_error(L, "Failed to dump callback");
    }

    if (chunkListLoad(sCallbackState, &chunkList, "callback") != 0) {
        criticalSectionLeave(sCriticalSection);
        return lua_error(L);
    }

    chunkListFree(&chunkList);

    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    lua_pushlightuserdata(sCallbackState, owner);
    lua_pushvalue(sCallbackState, -3);
    lua_settable(sCallbackState, -3);

    int reference = luaL_ref(sCallbackState, LUA_REGISTRYINDEX);

    criticalSectionLeave(sCriticalSection);

    return reference;
}

static int isSimpleType(int type)
{
    return type == LUA_TNIL
        || type == LUA_TBOOLEAN
        || type == LUA_TLIGHTUSERDATA
        || type == LUA_TNUMBER
        || type == LUA_TSTRING;
}

static int checkUserDataTable(lua_State *L, int currentDepth, int maximumDepth);

/* Expects the value to check at the top of the stack, and leaves it on the stack */
static int checkUserDataField(lua_State *L, int currentDepth, int maximumDepth)
{
    int type = lua_type(L, -1);

    if (isSimpleType(type)) {
        return 0;
    } else if (type == LUA_TTABLE) {
        if (currentDepth < maximumDepth) {
            return checkUserDataTable(L, currentDepth + 1, maximumDepth);
        } else {
            return luaL_error(L, "Too many levels of table nesting in userdata (maximum is %d)", maximumDepth);
        }
    } else {
        return luaL_error(L, "Unsupported userdata type: %s", lua_typename(L, type));
    }
}

/* Expects the value to check at the top of the stack, and leaves it on the stack */
static int checkUserDataTable(lua_State *L, int currentDepth, int maximumDepth)
{
    if (lua_getmetatable(L, -1)) {
        return luaL_error(L, "Userdata with a metatable is not supported");
    }

    lua_pushnil(L);

    while (lua_next(L, -2) != 0) {
        /* Check the value */
        checkUserDataField(L, currentDepth, maximumDepth);

        lua_pop(L, 1);

        /* Check the key */
        checkUserDataField(L, currentDepth, maximumDepth);
    }

    return 0;
}

#if 0
int checkUserData(lua_State *L, int index)
{
    int type = lua_type(L, index);

    if (type == LUA_TNONE) {
        return 0;
    } else {
        lua_pushvalue(L, index);
        int result = checkUserDataField(L, 0, 2);
        lua_pop(L, 1);
        return result;
    }
}
#endif

static int chunkListPrintf(lua_State *L, ChunkList *chunkList, const char *format, ...)
{
    if (!chunkList) {
        return 0;
    }

    enum { FixedBufferSize = 32, MaximumBufferSize = 1024 * 1024 };
    static char FixedBuffer[FixedBufferSize] = { 0 };

    static int sBufferSize = FixedBufferSize;
    static char *sBuffer = FixedBuffer;

    va_list args;
    va_start(args, format);

    int bytesWritten = vsnprintf(sBuffer, sBufferSize, format, args) + 1;

    va_end(args);

    if (sBufferSize < bytesWritten) {
        if (bytesWritten > MaximumBufferSize) {
            return luaL_error(L, "Userdata field is too long (%d bytes)", bytesWritten);
        }

        int newBufferSize = sBufferSize;

        while (newBufferSize < bytesWritten) {
            newBufferSize *= 2;
        }

        free(sBuffer);
        sBuffer = malloc(sizeof(*sBuffer) * newBufferSize);

        if (!sBuffer) {
            sBuffer = FixedBuffer;
            sBufferSize = FixedBufferSize;

            return luaL_error(L, "Failed allocating %d bytes for serialisation buffer", newBufferSize);
        }

        sBufferSize = newBufferSize;

        va_start(args, format);

        bytesWritten = vsnprintf(sBuffer, sBufferSize, format, args) + 1;

        va_end(args);

        if (sBufferSize < bytesWritten) {
            return luaL_error(L, "Failed serialising userdata field with format \"%s\"", format);
        }
    }

    /* Skip null terminator */
    --bytesWritten;

    if (chunkListWrite(L, sBuffer, bytesWritten, chunkList) != 0) {
        return luaL_error(L, "Failed serialising userdata field: \"%s\"", sBuffer);
    }

    return 0;
}

static int serialiseUserDataTable(lua_State *L, ChunkList *chunkList, int currentDepth, int maximumDepth);

static int serialiseUserDataField(lua_State *L, ChunkList *chunkList, int currentDepth, int maximumDepth)
{
    int type = lua_type(L, -1);

    switch (type) {
        case LUA_TNIL:
            chunkListPrintf(L, chunkList, "nil");
            break;
        case LUA_TBOOLEAN:
            chunkListPrintf(L, chunkList, "%s", lua_toboolean(L, -1) ? "true" : "false");
            break;
        case LUA_TLIGHTUSERDATA:
            chunkListPrintf(L, chunkList, "%p", lua_touserdata(L, -1));
            break;
        case LUA_TNUMBER:
            chunkListPrintf(L, chunkList, "%f", lua_tonumber(L, -1));
            break;
        case LUA_TSTRING:
            chunkListPrintf(L, chunkList, "\"%s\"", lua_tostring(L, -1));
            break;
        case LUA_TTABLE:
            if (currentDepth < maximumDepth) {
                return serialiseUserDataTable(L, chunkList, currentDepth + 1, maximumDepth);
            } else {
                return luaL_error(L, "Too many levels of table nesting in userdata (maximum is %d)", maximumDepth);
            }
            break;
        default:
            return luaL_error(L, "Unsupported userdata type: %s", lua_typename(L, type));
    }

    return 0;
}

static int serialiseUserDataTable(lua_State *L, ChunkList *chunkList, int currentDepth, int maximumDepth)
{
    if (lua_getmetatable(L, -1)) {
        return luaL_error(L, "Userdata with a metatable is not supported");
    }

    chunkListPrintf(L, chunkList, "{\n");

    lua_pushnil(L);

    while (lua_next(L, -2) != 0) {
        /* Serialise the key */
        lua_pushvalue(L, -2);
        chunkListPrintf(L, chunkList, "[");
        serialiseUserDataField(L, chunkList, currentDepth, maximumDepth);
        chunkListPrintf(L, chunkList, "] = ");
        lua_pop(L, 1);

        /* Serialise the value */
        serialiseUserDataField(L, chunkList, currentDepth, maximumDepth);
        chunkListPrintf(L, chunkList, ",\n");

        lua_pop(L, 1);
    }

    chunkListPrintf(L, chunkList, "}");

    return 0;
}

static int serialiseUserData(lua_State *L, int index, ChunkList *chunkList)
{
    int type = lua_type(L, index);

    if (type == LUA_TNONE) {
        return 0;
    } else {
        chunkListPrintf(L, chunkList, "return ");

        lua_pushvalue(L, index);
        int result = serialiseUserDataField(L, chunkList, 0, 2);
        lua_pop(L, 1);

        chunkListPrintf(L, chunkList, "\n");

        return result;
    }
}

int callbacks_checkUserData(lua_State *L, int index)
{
    return serialiseUserData(L, index, NULL);
}

int callbacks_setUserData(lua_State *L, int index, void *owner)
{
    REQUIRE_OK(affirmCriticalSection());

    criticalSectionEnter(sCriticalSection);

#define CHECK_RESULT(r) \
    do { \
        FMOD_RESULT _r = (r); \
        if (_r != FMOD_OK) { \
            criticalSectionLeave(sCriticalSection); \
            REQUIRE_OK(_r); \
        } \
    } while (0)

    CHECK_RESULT(affirmCallbackState(L));

    affirmUserdataTable();

    ChunkList chunkList = { 0 };

    int result = serialiseUserData(L, index, &chunkList);

    if (result != 0) {
        chunkListFree(&chunkList);
        return result;
    }

    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, USERDATA_TABLE);
    lua_pushlightuserdata(sCallbackState, owner);

    if (chunkListLoad(sCallbackState, &chunkList, "userdata") != 0) {
        criticalSectionLeave(sCriticalSection);
        return lua_error(L);
    }

    if (lua_pcall(sCallbackState, 0, 1, 0) != 0) {
        criticalSectionLeave(sCriticalSection);
        return lua_error(L);
    }

    lua_settable(sCallbackState, -3);

    criticalSectionLeave(sCriticalSection);

    chunkListFree(&chunkList);

    return 0;
}

int callbacks_getUserData(lua_State *L, void *owner)
{
    lua_getfield(L, LUA_REGISTRYINDEX, USERDATA_TABLE);

    if (!lua_istable(L, -1)) {
        return 0;
    }

    lua_pushlightuserdata(L, owner);
    lua_gettable(L, -2);

    return 1;
}
