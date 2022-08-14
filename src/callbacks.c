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

static void affirmCallbacksTable()
{
    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);

    if (lua_isnil(sCallbackState, -1)) {
        lua_newtable(sCallbackState);
        lua_setfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);
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

static int chunkListWriter(lua_State *L, const void *p, size_t size, void *userdata)
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

static const char *chunkListReader(lua_State *L, void *userdata, size_t *size)
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

FMOD_RESULT F_CALLBACK eventCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event,
    void *parameters)
{
    criticalSectionEnter(sCriticalSection);

    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    lua_pushlightuserdata(sCallbackState, event);
    lua_gettable(sCallbackState, -2);

    if (!lua_isnil(sCallbackState, -1)) {
        CONSTANT_access_FMOD_STUDIO_EVENT_CALLBACK_TYPE(sCallbackState, &type, 0, 0);

        if (lua_pcall(sCallbackState, 1, 0, 0) != 0) {
            const char *error = lua_tostring(sCallbackState, -1);
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

    if (lua_dump(L, chunkListWriter, &chunkList) != 0) {
        criticalSectionLeave(sCriticalSection);
        return luaL_error(L, "Failed to dump callback");
    }

    Chunk *currentChunk = chunkList.first;

    if (lua_load(sCallbackState, chunkListReader, &currentChunk, "callback") != 0) {
        criticalSectionLeave(sCriticalSection);
        return lua_error(L);
    }

    for (currentChunk = chunkList.first; currentChunk;)
    {
        Chunk *previous = currentChunk;

        currentChunk = currentChunk->next;

        free(previous);
    }

    lua_getfield(sCallbackState, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    lua_pushlightuserdata(sCallbackState, owner);
    lua_pushvalue(sCallbackState, -3);
    lua_settable(sCallbackState, -3);

    int reference = luaL_ref(sCallbackState, LUA_REGISTRYINDEX);

    criticalSectionLeave(sCriticalSection);

    return reference;
}
