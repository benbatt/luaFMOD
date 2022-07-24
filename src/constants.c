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

#include <fmod_studio_common.h>
#include <lauxlib.h>

struct Constant {
    const char *name;
    unsigned int value;
};

#define JOIN(a, b) JOIN_IMPL(a, b)
#define JOIN4(a, b, c, d) JOIN(JOIN(a, b), JOIN(c, d))

#define JOIN_IMPL(a, b) a ## b

#define STRINGIZE(x) STRINGIZE_IMPL(x)

#define STRINGIZE_IMPL(x) #x

// redefine CONSTANT_TABLE_NAME before each table
#define CONSTANT_TABLE_BEGIN \
    do { \
        lua_createtable(L, 0, 0); \
    } while(0)

#define CONSTANT_TABLE_ENTRY(name) \
    do { \
        lua_pushinteger(L, JOIN4(FMOD_STUDIO_, CONSTANT_TABLE_NAME, _, name)); \
        lua_setfield(L, -2, #name); \
    } while(0)

#define CONSTANT_TABLE_END \
    do { \
        lua_setfield(L, -2, STRINGIZE(CONSTANT_TABLE_NAME)); \
    } while(0)

void createConstantTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack; get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

#define CONSTANT_TABLE_NAME INIT

    CONSTANT_TABLE_BEGIN;
        CONSTANT_TABLE_ENTRY(NORMAL);
        CONSTANT_TABLE_ENTRY(LIVEUPDATE);
        CONSTANT_TABLE_ENTRY(ALLOW_MISSING_PLUGINS);
        CONSTANT_TABLE_ENTRY(SYNCHRONOUS_UPDATE);
        CONSTANT_TABLE_ENTRY(DEFERRED_CALLBACKS);
        CONSTANT_TABLE_ENTRY(LOAD_FROM_UPDATE);
        CONSTANT_TABLE_ENTRY(MEMORY_TRACKING);
    CONSTANT_TABLE_END;

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
