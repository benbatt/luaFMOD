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

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "logging.h"
#include "platform.h"

typedef struct LogRecord {
    FMOD_DEBUG_FLAGS flags;
    char *func;
    char *message;
} LogRecord;

enum { CHUNK_SIZE = 32 };

typedef struct Chunk {
    LogRecord records[CHUNK_SIZE];
    struct Chunk *next;
    int count;
} Chunk;

static LUAFMOD_CRITICAL_SECTION *sCriticalSection = NULL;

static Chunk *sCurrentChunk = NULL;

static Chunk *sFirstFullChunk = NULL;
static Chunk *sLastFullChunk = NULL;

static int sChunkCount = 0;

static int sLuaCallback = 0;

static int addChunk()
{
    Chunk *chunk = malloc(sizeof(*chunk));

    if (!chunk) {
        return 0;
    }

    chunk->count = 0;

    chunk->next = sCurrentChunk;
    sCurrentChunk = chunk;

    ++sChunkCount;

    return 1;
}

static void moveCurrentToFullQueue()
{
    Chunk *chunk = sCurrentChunk;
    sCurrentChunk = chunk->next;

    chunk->next = NULL;

    if (!sFirstFullChunk) {
        sFirstFullChunk = chunk;
    }

    if (sLastFullChunk) {
        sLastFullChunk->next = chunk;
    }

    sLastFullChunk = chunk;
}

static FMOD_RESULT F_CALL debugCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line,
    const char *func, const char *message)
{
    criticalSectionEnter(sCriticalSection);

    if (!sCurrentChunk) {
        if (!addChunk()) {
            criticalSectionLeave(sCriticalSection);
            return FMOD_ERR_MEMORY;
        }
    }

    Chunk *chunk = sCurrentChunk;

    LogRecord *record = &chunk->records[chunk->count];
    record->flags = flags;
    record->func = _strdup(func);

    if (!record->func) {
        criticalSectionLeave(sCriticalSection);
        return FMOD_ERR_MEMORY;
    }

    record->message = _strdup(message);

    if (!record->message) {
        criticalSectionLeave(sCriticalSection);
        return FMOD_ERR_MEMORY;
    }

    ++chunk->count;

    if (chunk->count == CHUNK_SIZE) {
        moveCurrentToFullQueue();
    }

    criticalSectionLeave(sCriticalSection);
    return FMOD_OK;
}

FMOD_RESULT loggingStart(FMOD_DEBUG_FLAGS flags, FMOD_DEBUG_MODE mode, int luaCallback, const char *filename)
{
    FMOD_DEBUG_CALLBACK callback = NULL;

    if (mode == FMOD_DEBUG_MODE_CALLBACK && luaCallback) {
        sCriticalSection = criticalSectionCreate();

        if (!sCriticalSection) {
            return FMOD_ERR_MEMORY;
        }

        if (!addChunk()) {
            return FMOD_ERR_MEMORY;
        }

        sLuaCallback = luaCallback;

        callback = debugCallback;
    }

    return FMOD_Debug_Initialize(flags, mode, callback, filename);
}

/* Leaves the stack unchanged.
*/
static void pumpChunkMessages(lua_State *L, Chunk *chunk)
{
    int i = 0;

    for (i = 0; i < chunk->count; ++i) {
        LogRecord *record = &chunk->records[i];

        lua_rawgeti(L, LUA_REGISTRYINDEX, sLuaCallback);
        PUSH_CONSTANT(L, FMOD_DEBUG_FLAGS, record->flags);
        lua_pushstring(L, record->func);
        lua_pushstring(L, record->message);

        lua_call(L, 3, 0);

        free(record->func);
        free(record->message);
    }

    chunk->count = 0;
}

/* Leaves the stack unchanged.
*/
void loggingPumpMessages(lua_State *L)
{
    if (!sCriticalSection) {
        return;
    }

    criticalSectionEnter(sCriticalSection);

    if (sCurrentChunk) {
        moveCurrentToFullQueue();
    }

    Chunk *firstChunk = sFirstFullChunk;
    Chunk *lastChunk = sLastFullChunk;

    sFirstFullChunk = NULL;
    sLastFullChunk = NULL;

    criticalSectionLeave(sCriticalSection);

    if (firstChunk) {
        for (Chunk *chunk = firstChunk; chunk; chunk = chunk->next) {
            pumpChunkMessages(L, chunk);
        }

        criticalSectionEnter(sCriticalSection);

        if (sCurrentChunk) {
            lastChunk->next = sCurrentChunk->next;
            sCurrentChunk->next = firstChunk;
        } else {
            sCurrentChunk = firstChunk;
        }

        criticalSectionLeave(sCriticalSection);
    }
}
