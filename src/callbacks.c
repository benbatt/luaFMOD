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
        PUSH_CONSTANT(L, FMOD_STUDIO_EVENT_CALLBACK_TYPE, type);
        PUSH_HANDLE(L, FMOD_STUDIO_EVENTINSTANCE, event);

        if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND) {
            FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES_newref(L, 0, (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters);
        } else {
            lua_pushnil(L);
        }

        if (lua_pcall(L, 3, 0, 0) != 0) {
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

static int copyUserDataTable(lua_State *source, lua_State *destination, int currentDepth, int maximumDepth);

static int copyFMODHandle(lua_State *source, lua_State *destination, int currentDepth, int maximumDepth)
{
    if (lua_getmetatable(source, -1) != 0) {
        luaL_getmetatable(source, "FMOD_SOUND");

        if (lua_rawequal(source, -1, -2)) {
            lua_pop(source, 2);

            if (destination) {
                FMOD_SOUND *sound = *((FMOD_SOUND**)lua_touserdata(source, -1));
                PUSH_HANDLE(destination, FMOD_SOUND, sound);
            }

            return 0;
        }
    }

    return luaL_error(source, "Unrecognised userdata type");
}

static int copyUserDataField(lua_State *source, lua_State *destination, int currentDepth, int maximumDepth)
{
    int type = lua_type(source, -1);

    switch (type) {
        case LUA_TNIL:
            if (destination) {
                lua_pushnil(destination);
            }
            break;
        case LUA_TBOOLEAN:
            if (destination) {
                lua_pushboolean(destination, lua_toboolean(source, -1));
            }
            break;
        case LUA_TLIGHTUSERDATA:
            if (destination) {
                lua_pushlightuserdata(destination, lua_touserdata(source, -1));
            }
            break;
        case LUA_TNUMBER:
            if (destination) {
                lua_pushnumber(destination, lua_tonumber(source, -1));
            }
            break;
        case LUA_TSTRING:
            if (destination) {
                size_t length = 0;
                const char *string = lua_tolstring(source, -1, &length);
                lua_pushlstring(destination, string, length);
            }
            break;
        case LUA_TTABLE:
            if (currentDepth < maximumDepth) {
                return copyUserDataTable(source, destination, currentDepth + 1, maximumDepth);
            } else {
                return luaL_error(source, "Too many levels of table nesting in userdata (maximum is %d)", maximumDepth);
            }
            break;
        case LUA_TUSERDATA:
            return copyFMODHandle(source, destination, currentDepth, maximumDepth);
        default:
            return luaL_error(source, "Unsupported userdata type: %s", lua_typename(source, type));
    }

    return 0;
}

static int copyUserDataTable(lua_State *source, lua_State *destination, int currentDepth, int maximumDepth)
{
    if (lua_getmetatable(source, -1)) {
        return luaL_error(source, "Userdata with a metatable is not supported");
    }

    if (destination) {
        lua_newtable(destination);
    }

    lua_pushnil(source);

    while (lua_next(source, -2) != 0) {
        /* Copy the key */
        lua_pushvalue(source, -2);
        copyUserDataField(source, destination, currentDepth, maximumDepth);
        lua_pop(source, 1);

        /* Copy the value */
        copyUserDataField(source, destination, currentDepth, maximumDepth);
        lua_pop(source, 1);

        /* Set the destination table field */
        if (destination) {
            lua_settable(destination, -3);
        }
    }

    return 0;
}

static int copyUserData(lua_State *source, int index, lua_State *destination)
{
    int type = lua_type(source, index);

    if (type == LUA_TNONE) {
        return 0;
    } else {
        lua_pushvalue(source, index);
        int result = copyUserDataField(source, destination, 0, 2);
        lua_pop(source, 1);

        return result;
    }
}

int callbacks_checkUserData(lua_State *L, int index)
{
    return copyUserData(L, index, NULL);
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

    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, USERDATA_TABLE);
    lua_pushlightuserdata(sCallbackState, owner);

    copyUserData(L, index, sCallbackState);

    lua_settable(sCallbackState, -3);

    criticalSectionLeave(sCriticalSection);

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
