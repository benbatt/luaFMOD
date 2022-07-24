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

#include "common.h"

struct Constant {
    const char *name;
    unsigned int value;
};

#define JOIN(a, b) JOIN_IMPL(a, b)
#define JOIN4(a, b, c, d) JOIN(JOIN(a, b), JOIN(c, d))

#define JOIN_IMPL(a, b) a ## b

#define TABLE_ENTRY_VALUE(name) JOIN4(FMOD_, TABLE_NAME, _, name)

#define TABLE_BEGIN(name, createMetatable, metatableName) \
    do { \
        createMetatable(L, metatableName); \
        lua_createtable(L, 0, 0); \
        lua_pushvalue(L, -1); \
        lua_setfield(L, -3, #name); \
    } while(0)

#define TABLE_ENTRY(name, metatable) \
    do { \
        *(int*)lua_newuserdata(L, sizeof(int)) = TABLE_ENTRY_VALUE(name); \
        luaL_getmetatable(L, metatable); \
        lua_setmetatable(L, -2); \
        lua_setfield(L, -2, #name); \
    } while(0)

#define TABLE_END \
    do { \
        lua_pop(L, 1); \
    } while(0)

/* Begins a new flags table.
   TABLE_NAME must be redefined before each table.
*/

#define FLAGS_TABLE_BEGIN(name) TABLE_BEGIN(name, createFlagsMetatable, FLAGS_METATABLE_NAME(TABLE_NAME))
#define FLAGS_TABLE_ENTRY(name) TABLE_ENTRY(name, FLAGS_METATABLE_NAME(TABLE_NAME))

void createFlagsMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);
    lua_pop(L, 1);
}

/* Begins a new enum table.
   TABLE_NAME must be redefined before each table.
*/

#define ENUM_TABLE_BEGIN(name) TABLE_BEGIN(name, createEnumMetatable, ENUM_METATABLE_NAME(TABLE_NAME))
#define ENUM_TABLE_ENTRY(name) TABLE_ENTRY(name, ENUM_METATABLE_NAME(TABLE_NAME))

void createEnumMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);
    lua_pop(L, 1);
}

void createConstantTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack, so define FMOD constants first */

#define TABLE_NAME INIT

    FLAGS_TABLE_BEGIN(INIT);
        FLAGS_TABLE_ENTRY(NORMAL);
        FLAGS_TABLE_ENTRY(STREAM_FROM_UPDATE);
        FLAGS_TABLE_ENTRY(MIX_FROM_UPDATE);
        FLAGS_TABLE_ENTRY(3D_RIGHTHANDED);
        FLAGS_TABLE_ENTRY(CHANNEL_LOWPASS);
        FLAGS_TABLE_ENTRY(CHANNEL_DISTANCEFILTER);
        FLAGS_TABLE_ENTRY(PROFILE_ENABLE);
        FLAGS_TABLE_ENTRY(VOL0_BECOMES_VIRTUAL);
        FLAGS_TABLE_ENTRY(GEOMETRY_USECLOSEST);
        FLAGS_TABLE_ENTRY(PREFER_DOLBY_DOWNMIX);
        FLAGS_TABLE_ENTRY(THREAD_UNSAFE);
        FLAGS_TABLE_ENTRY(PROFILE_METER_ALL);
        FLAGS_TABLE_ENTRY(MEMORY_TRACKING);
    TABLE_END;

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

#undef TABLE_NAME
#define TABLE_NAME STUDIO_INIT

    FLAGS_TABLE_BEGIN(INIT);
        FLAGS_TABLE_ENTRY(NORMAL);
        FLAGS_TABLE_ENTRY(LIVEUPDATE);
        FLAGS_TABLE_ENTRY(ALLOW_MISSING_PLUGINS);
        FLAGS_TABLE_ENTRY(SYNCHRONOUS_UPDATE);
        FLAGS_TABLE_ENTRY(DEFERRED_CALLBACKS);
        FLAGS_TABLE_ENTRY(LOAD_FROM_UPDATE);
        FLAGS_TABLE_ENTRY(MEMORY_TRACKING);
    TABLE_END;

#undef TABLE_NAME
#define TABLE_NAME STUDIO_LOAD_BANK

    FLAGS_TABLE_BEGIN(LOAD_BANK);
        FLAGS_TABLE_ENTRY(NORMAL);
        FLAGS_TABLE_ENTRY(NONBLOCKING);
        FLAGS_TABLE_ENTRY(DECOMPRESS_SAMPLES);
        FLAGS_TABLE_ENTRY(UNENCRYPTED);
    TABLE_END;

#undef TABLE_NAME
#define TABLE_NAME STUDIO_STOP

    ENUM_TABLE_BEGIN(STOP);
        ENUM_TABLE_ENTRY(ALLOWFADEOUT);
        ENUM_TABLE_ENTRY(IMMEDIATE);
    TABLE_END;

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
