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

/* Expects (parent) on the stack.
   Creates a new constants table and sets it as parent[name].
   Leaves (parent, constants table) on the stack.
*/
static void CONSTANT_TABLE_create(lua_State *L, const char *name)
{
    lua_createtable(L, 0, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, name);
}

static int CONSTANT_new(lua_State *L, const char *metatable, int value)
{
    *(int*)lua_newuserdata(L, sizeof(int)) = value;

    luaL_getmetatable(L, metatable);

    if (lua_isnoneornil(L, -1))
    {
        return luaL_error(L, "The metatable for %s has not been defined", metatable);
    }

    lua_setmetatable(L, -2);

    return 1;
}

#define TABLE_ENTRY_VALUE(name) JOIN(TABLE_VALUE_PREFIX, name)

/* Expects parent on the stack.
   Creates a new constants table and sets it as parent[name].
   Leaves the stack unchanged.
*/
#define TABLE_BEGIN(type, createMetatable) \
    CONSTANT_ACCESS(type) \
    TABLE_CREATE(type, createMetatable)

#define CONSTANT_ACCESS(type) \
    CONSTANT_ACCESS_DECLARE(type) \
    { \
        if (set) \
        { \
            *data = CHECK_CONSTANT(valueIndex, type); \
            return 0; \
        } \
        else \
        { \
            return CONSTANT_new(L, # type, *data); \
        } \
    }

/* Expects (parent) on the stack.
   Creates a new constants table and sets it as parent[name].
   Creates a new instance metatable named "type".
   Leaves (parent, constants table) on the stack.
*/
#define TABLE_CREATE(type, createMetatable) \
    static void type ## _table_create(lua_State *L, const char *fieldName) \
    { \
        CONSTANT_TABLE_create(L, fieldName); \
        const char *metatable = # type; \
        createMetatable(L, metatable);

/* Expects (constants table) on the stack.
   Creates a new table entry and sets it as (constants table)[name].
   Leaves the stack unchanged.
*/
#define TABLE_ENTRY(name) \
        CONSTANT_new(L, metatable, TABLE_ENTRY_VALUE(name)); \
        lua_setfield(L, -2, #name);

/* Expects (parent, constants table) on the stack.
   Leaves (parent) on the stack.
*/
#define TABLE_END \
        lua_pop(L, 1); \
    }

/* Begins a new flags table.
   TABLE_VALUE_PREFIX must be redefined before each table.
*/

#define FLAGS_TABLE_BEGIN(type) \
    TABLE_BEGIN(type, createFlagsMetatable)

/* Begins a new enum table.
   TABLE_VALUE_PREFIX must be redefined before each table.
*/

#define ENUM_TABLE_BEGIN(type) \
    TABLE_BEGIN(type, createEnumMetatable)

int combineFlags(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TUSERDATA);

    lua_getmetatable(L, 1);
    lua_getmetatable(L, 2);

    if (!lua_rawequal(L, -1, -2))
    {
        luaL_error(L, "Attempt to combine flags of different types");
    }

    int result = (*(int*)lua_touserdata(L, 1)) | (*(int*)lua_touserdata(L, 2));

    *(int*)lua_newuserdata(L, sizeof(result)) = result;

    lua_getmetatable(L, 1);
    lua_setmetatable(L, -2);

    return 1;
}

void createFlagsMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);

    lua_pushcfunction(L, combineFlags);
    lua_setfield(L, -2, "__add");

    lua_pop(L, 1);
}

void createEnumMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);
    lua_pop(L, 1);
}

#define TABLE_VALUE_PREFIX FMOD_INIT_

FLAGS_TABLE_BEGIN(FMOD_INITFLAGS)
    TABLE_ENTRY(NORMAL)
    TABLE_ENTRY(STREAM_FROM_UPDATE)
    TABLE_ENTRY(MIX_FROM_UPDATE)
    TABLE_ENTRY(3D_RIGHTHANDED)
    TABLE_ENTRY(CHANNEL_LOWPASS)
    TABLE_ENTRY(CHANNEL_DISTANCEFILTER)
    TABLE_ENTRY(PROFILE_ENABLE)
    TABLE_ENTRY(VOL0_BECOMES_VIRTUAL)
    TABLE_ENTRY(GEOMETRY_USECLOSEST)
    TABLE_ENTRY(PREFER_DOLBY_DOWNMIX)
    TABLE_ENTRY(THREAD_UNSAFE)
    TABLE_ENTRY(PROFILE_METER_ALL)
    TABLE_ENTRY(MEMORY_TRACKING)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_SPEAKERMODE_

ENUM_TABLE_BEGIN(FMOD_SPEAKERMODE)
    TABLE_ENTRY(DEFAULT)
    TABLE_ENTRY(RAW)
    TABLE_ENTRY(MONO)
    TABLE_ENTRY(STEREO)
    TABLE_ENTRY(QUAD)
    TABLE_ENTRY(SURROUND)
    TABLE_ENTRY(5POINT1)
    TABLE_ENTRY(7POINT1)
    TABLE_ENTRY(7POINT1POINT4)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_STUDIO_INIT_

FLAGS_TABLE_BEGIN(FMOD_STUDIO_INITFLAGS)
    TABLE_ENTRY(NORMAL)
    TABLE_ENTRY(LIVEUPDATE)
    TABLE_ENTRY(ALLOW_MISSING_PLUGINS)
    TABLE_ENTRY(SYNCHRONOUS_UPDATE)
    TABLE_ENTRY(DEFERRED_CALLBACKS)
    TABLE_ENTRY(LOAD_FROM_UPDATE)
    TABLE_ENTRY(MEMORY_TRACKING)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_STUDIO_PARAMETER_

FLAGS_TABLE_BEGIN(FMOD_STUDIO_PARAMETER_FLAGS)
    TABLE_ENTRY(READONLY)
    TABLE_ENTRY(AUTOMATIC)
    TABLE_ENTRY(GLOBAL)
    TABLE_ENTRY(DISCRETE)
    TABLE_ENTRY(LABELED)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_STUDIO_LOAD_BANK_

FLAGS_TABLE_BEGIN(FMOD_STUDIO_LOAD_BANK_FLAGS)
    TABLE_ENTRY(NORMAL)
    TABLE_ENTRY(NONBLOCKING)
    TABLE_ENTRY(DECOMPRESS_SAMPLES)
    TABLE_ENTRY(UNENCRYPTED)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_STUDIO_PARAMETER_

ENUM_TABLE_BEGIN(FMOD_STUDIO_PARAMETER_TYPE)
    TABLE_ENTRY(GAME_CONTROLLED)
    TABLE_ENTRY(AUTOMATIC_DISTANCE)
    TABLE_ENTRY(AUTOMATIC_EVENT_CONE_ANGLE)
    TABLE_ENTRY(AUTOMATIC_EVENT_ORIENTATION)
    TABLE_ENTRY(AUTOMATIC_DIRECTION)
    TABLE_ENTRY(AUTOMATIC_ELEVATION)
    TABLE_ENTRY(AUTOMATIC_LISTENER_ORIENTATION)
    TABLE_ENTRY(AUTOMATIC_SPEED)
    TABLE_ENTRY(AUTOMATIC_SPEED_ABSOLUTE)
    TABLE_ENTRY(AUTOMATIC_DISTANCE_NORMALIZED)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_STUDIO_STOP_

ENUM_TABLE_BEGIN(FMOD_STUDIO_STOP_MODE)
    TABLE_ENTRY(ALLOWFADEOUT)
    TABLE_ENTRY(IMMEDIATE)
TABLE_END

void createConstantTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack, so define FMOD constants first */
    FMOD_INITFLAGS_table_create(L, "INIT");
    FMOD_SPEAKERMODE_table_create(L, "SPEAKERMODE");

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

    FMOD_STUDIO_INITFLAGS_table_create(L, "INIT");
    FMOD_STUDIO_PARAMETER_FLAGS_table_create(L, "PARAMETER_FLAGS");
    FMOD_STUDIO_LOAD_BANK_FLAGS_table_create(L, "LOAD_BANK");
    FMOD_STUDIO_PARAMETER_TYPE_table_create(L, "PARAMETER_TYPE");
    FMOD_STUDIO_STOP_MODE_table_create(L, "STOP");

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
