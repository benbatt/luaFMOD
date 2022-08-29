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

int getOptionalConstant(lua_State *L, int index, const char *metatable, int defaultValue)
{
    if (lua_isnoneornil(L, index)) {
        return defaultValue;
    } else {
        return *(int*)luaL_checkudata(L, index, metatable);
    }
}

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

int CONSTANT_new(lua_State *L, const char *metatable, int value)
{
    *(int*)lua_newuserdata(L, sizeof(int)) = value;

    luaL_getmetatable(L, metatable);

    if (lua_isnoneornil(L, -1)) {
        return luaL_error(L, "The metatable for %s has not been defined", metatable);
    }

    lua_setmetatable(L, -2);

    return 1;
}

static void CONSTANT_TABLE_entry(lua_State *L, const char *metatable, const char *name, int value)
{
    CONSTANT_new(L, metatable, value);
    lua_setfield(L, -2, name);

    /* Set metatable[value] = name for __tostring to use */
    luaL_getmetatable(L, metatable);
    lua_pushinteger(L, value);
    lua_pushstring(L, name);
    lua_settable(L, -3);

    /* Tidy up the metatable */
    lua_pop(L, 1);
}

#define TABLE_ENTRY_VALUE(name) JOIN(TABLE_VALUE_PREFIX, name)

/* Expects parent on the stack.
   Creates a new constants table and sets it as parent[name].
   Leaves the stack unchanged.
*/
#define TABLE_BEGIN(type, createMetatable) \
    CONSTANT_ACCESS(type) \
    TABLE_CREATE_BEGIN(type, createMetatable)

#define CONSTANT_ACCESS(type) \
    CONSTANT_ACCESS_DECLARE(type) \
    { \
        if (set) { \
            *data = CHECK_CONSTANT(valueIndex, type); \
            return 0; \
        } else { \
            return CONSTANT_new(L, # type, *data); \
        } \
    }

/* Expects (parent) on the stack.
   Creates a new constants table and sets it as parent[name].
   Creates a new instance metatable named "type".
   Leaves (parent, constants table) on the stack.
*/
#define TABLE_CREATE_BEGIN(type, createMetatable) \
    static void CONSTANT_TABLE_create_ ## type(lua_State *L, const char *fieldName) \
    { \
        CONSTANT_TABLE_create(L, fieldName); \
        const char *metatable = # type; \
        createMetatable(L, metatable);

/* Expects (constants table) on the stack.
   Creates a new table entry and sets it as (constants table)[name].
   Leaves the stack unchanged.
*/
#define TABLE_ENTRY(name) \
    CONSTANT_TABLE_entry(L, metatable, # name, TABLE_ENTRY_VALUE(name));

/* Same as TABLE_ENTRY, but name is a string and value is an int. */
#define TABLE_ENTRY_CUSTOM(name, value) \
    CONSTANT_TABLE_entry(L, metatable, name, value);

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

/* Expects (parent) on the stack.
   Creates the constants table defined for type and sets it as parent[name].
   Leaves (parent) on the stack.
*/
#define TABLE_CREATE(type, name) CONSTANT_TABLE_create_ ## type(L, name)

int equalFlags(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TUSERDATA);

    lua_getmetatable(L, 1);
    lua_getmetatable(L, 2);

    if (!lua_rawequal(L, -1, -2)) {
        luaL_error(L, "Attempt to compare flags of different types");
    }

    int value1 = (*(int*)lua_touserdata(L, 1));
    int value2 = (*(int*)lua_touserdata(L, 2));

    lua_pushboolean(L, value1 == value2);

    return 1;
}

int combineFlags(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TUSERDATA);

    lua_getmetatable(L, 1);
    lua_getmetatable(L, 2);

    if (!lua_rawequal(L, -1, -2)) {
        luaL_error(L, "Attempt to combine flags of different types");
    }

    int result = (*(int*)lua_touserdata(L, 1)) | (*(int*)lua_touserdata(L, 2));

    *(int*)lua_newuserdata(L, sizeof(result)) = result;

    lua_getmetatable(L, 1);
    lua_setmetatable(L, -2);

    return 1;
}

int testFlags(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TUSERDATA);

    lua_getmetatable(L, 1);
    lua_getmetatable(L, 2);

    if (!lua_rawequal(L, -1, -2)) {
        luaL_error(L, "Attempt to combine flags of different types");
    }

    int result = (*(int*)lua_touserdata(L, 1)) & (*(int*)lua_touserdata(L, 2));

    lua_pushboolean(L, (result != 0));

    return 1;
}

int flagsToString(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);

    unsigned int value = *(unsigned int*)lua_touserdata(L, 1);

    if (value == 0) {
        lua_pushstring(L, "(none)");
        return 1;
    }

    lua_getmetatable(L, 1);
    int metatable = lua_gettop(L);

    luaL_Buffer buffer;
    luaL_buffinit(L, &buffer);

    luaL_addchar(&buffer, '(');

    int first = 1;

    for (unsigned int mask = 1; mask <= value; mask = mask << 1) {
        if ((mask & value) != 0) {
            if (!first) {
                luaL_addstring(&buffer, " + ");
            }

            lua_pushinteger(L, mask);
            lua_gettable(L, metatable);

            if (lua_isnil(L, -1)) {
                lua_pop(L, 1);
                lua_pushfstring(L, "(unknown: %d)", mask);
            }

            luaL_addvalue(&buffer);

            first = 0;
        }
    }

    luaL_addchar(&buffer, ')');

    luaL_pushresult(&buffer);

    return 1;
}

void createFlagsMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);

    lua_pushcfunction(L, equalFlags);
    lua_setfield(L, -2, "__eq");

    lua_pushcfunction(L, combineFlags);
    lua_setfield(L, -2, "__add");

    lua_pushcfunction(L, testFlags);
    lua_setfield(L, -2, "__mul");

    lua_pushcfunction(L, flagsToString);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);
}

int enumToString(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);

    int value = *(int*)lua_touserdata(L, 1);

    lua_getmetatable(L, 1);
    lua_pushinteger(L, value);
    lua_gettable(L, -2);

    if (lua_isnil(L, -1)) {
        lua_pushfstring(L, "unknown (%d)", value);
    }

    return 1;
}

void createEnumMetatable(lua_State *L, const char *name)
{
    luaL_newmetatable(L, name);

    lua_pushcfunction(L, enumToString);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);
}

#define TABLE_VALUE_PREFIX FMOD_DEBUG_

FLAGS_TABLE_BEGIN(FMOD_DEBUG_FLAGS)
    TABLE_ENTRY(LEVEL_NONE)
    TABLE_ENTRY(LEVEL_ERROR)
    TABLE_ENTRY(LEVEL_WARNING)
    TABLE_ENTRY(LEVEL_LOG)
    TABLE_ENTRY(TYPE_MEMORY)
    TABLE_ENTRY(TYPE_FILE)
    TABLE_ENTRY(TYPE_CODEC)
    TABLE_ENTRY(TYPE_TRACE)
    TABLE_ENTRY(DISPLAY_TIMESTAMPS)
    TABLE_ENTRY(DISPLAY_LINENUMBERS)
    TABLE_ENTRY(DISPLAY_THREAD)
TABLE_END

#undef TABLE_VALUE_PREFIX
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
#define TABLE_VALUE_PREFIX FMOD_TIMEUNIT_

ENUM_TABLE_BEGIN(FMOD_TIMEUNIT)
    TABLE_ENTRY(MS)
    TABLE_ENTRY(PCM)
    TABLE_ENTRY(PCMBYTES)
    TABLE_ENTRY(RAWBYTES)
    TABLE_ENTRY(PCMFRACTION)
    TABLE_ENTRY(MODORDER)
    TABLE_ENTRY(MODROW)
    TABLE_ENTRY(MODPATTERN)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_

FLAGS_TABLE_BEGIN(FMOD_MODE)
    TABLE_ENTRY(DEFAULT)
    TABLE_ENTRY(LOOP_OFF)
    TABLE_ENTRY(LOOP_NORMAL)
    TABLE_ENTRY(LOOP_BIDI)
    TABLE_ENTRY_CUSTOM("_2D", FMOD_2D)
    TABLE_ENTRY_CUSTOM("_3D", FMOD_3D)
    TABLE_ENTRY(CREATESTREAM)
    TABLE_ENTRY(CREATESAMPLE)
    TABLE_ENTRY(CREATECOMPRESSEDSAMPLE)
    TABLE_ENTRY(OPENUSER)
    TABLE_ENTRY(OPENMEMORY)
    TABLE_ENTRY(OPENMEMORY_POINT)
    TABLE_ENTRY(OPENRAW)
    TABLE_ENTRY(OPENONLY)
    TABLE_ENTRY(ACCURATETIME)
    TABLE_ENTRY(MPEGSEARCH)
    TABLE_ENTRY(NONBLOCKING)
    TABLE_ENTRY(UNIQUE)
    TABLE_ENTRY_CUSTOM("_3D_HEADRELATIVE", FMOD_3D_HEADRELATIVE)
    TABLE_ENTRY_CUSTOM("_3D_WORLDRELATIVE", FMOD_3D_WORLDRELATIVE)
    TABLE_ENTRY_CUSTOM("_3D_INVERSEROLLOFF", FMOD_3D_INVERSEROLLOFF)
    TABLE_ENTRY_CUSTOM("_3D_LINEARROLLOFF", FMOD_3D_LINEARROLLOFF)
    TABLE_ENTRY_CUSTOM("_3D_LINEARSQUAREROLLOFF", FMOD_3D_LINEARSQUAREROLLOFF)
    TABLE_ENTRY_CUSTOM("_3D_INVERSETAPEREDROLLOFF", FMOD_3D_INVERSETAPEREDROLLOFF)
    TABLE_ENTRY_CUSTOM("_3D_CUSTOMROLLOFF", FMOD_3D_CUSTOMROLLOFF)
    TABLE_ENTRY_CUSTOM("_3D_IGNOREGEOMETRY", FMOD_3D_IGNOREGEOMETRY)
    TABLE_ENTRY(IGNORETAGS)
    TABLE_ENTRY(LOWMEM)
    TABLE_ENTRY(VIRTUAL_PLAYFROMSTART)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_DEBUG_MODE_

ENUM_TABLE_BEGIN(FMOD_DEBUG_MODE)
    TABLE_ENTRY(TTY)
    /* Using TABLE_ENTRY(FILE) here would collide with the standard FILE type. */
    TABLE_ENTRY_CUSTOM("FILE", FMOD_DEBUG_MODE_FILE)
    TABLE_ENTRY(CALLBACK)
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
#define TABLE_VALUE_PREFIX FMOD_CHANNELORDER_

ENUM_TABLE_BEGIN(FMOD_CHANNELORDER)
    TABLE_ENTRY(DEFAULT)
    TABLE_ENTRY(WAVEFORMAT)
    TABLE_ENTRY(PROTOOLS)
    TABLE_ENTRY(ALLMONO)
    TABLE_ENTRY(ALLSTEREO)
    TABLE_ENTRY(ALSA)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_SOUND_TYPE_

ENUM_TABLE_BEGIN(FMOD_SOUND_TYPE)
    TABLE_ENTRY(UNKNOWN)
    TABLE_ENTRY(AIFF)
    TABLE_ENTRY(ASF)
    TABLE_ENTRY(DLS)
    TABLE_ENTRY(FLAC)
    TABLE_ENTRY(FSB)
    TABLE_ENTRY(IT)
    TABLE_ENTRY(MIDI)
    TABLE_ENTRY(MOD)
    TABLE_ENTRY(MPEG)
    TABLE_ENTRY(OGGVORBIS)
    TABLE_ENTRY(PLAYLIST)
    TABLE_ENTRY(RAW)
    TABLE_ENTRY(S3M)
    TABLE_ENTRY(USER)
    TABLE_ENTRY(WAV)
    TABLE_ENTRY(XM)
    TABLE_ENTRY(XMA)
    TABLE_ENTRY(AUDIOQUEUE)
    TABLE_ENTRY(AT9)
    TABLE_ENTRY(VORBIS)
    TABLE_ENTRY(MEDIA_FOUNDATION)
    TABLE_ENTRY(MEDIACODEC)
    TABLE_ENTRY(FADPCM)
    TABLE_ENTRY(OPUS)
TABLE_END

#undef TABLE_VALUE_PREFIX
#define TABLE_VALUE_PREFIX FMOD_SOUND_FORMAT_

ENUM_TABLE_BEGIN(FMOD_SOUND_FORMAT)
    TABLE_ENTRY(NONE)
    TABLE_ENTRY(PCM8)
    TABLE_ENTRY(PCM16)
    TABLE_ENTRY(PCM24)
    TABLE_ENTRY(PCM32)
    TABLE_ENTRY(PCMFLOAT)
    TABLE_ENTRY(BITSTREAM)
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
#define TABLE_VALUE_PREFIX FMOD_STUDIO_EVENT_CALLBACK_

FLAGS_TABLE_BEGIN(FMOD_STUDIO_EVENT_CALLBACK_TYPE)
    TABLE_ENTRY(CREATED)
    TABLE_ENTRY(DESTROYED)
    TABLE_ENTRY(STARTING)
    TABLE_ENTRY(STARTED)
    TABLE_ENTRY(RESTARTED)
    TABLE_ENTRY(STOPPED)
    TABLE_ENTRY(START_FAILED)
    TABLE_ENTRY(CREATE_PROGRAMMER_SOUND)
    TABLE_ENTRY(DESTROY_PROGRAMMER_SOUND)
    TABLE_ENTRY(PLUGIN_CREATED)
    TABLE_ENTRY(PLUGIN_DESTROYED)
    TABLE_ENTRY(TIMELINE_MARKER)
    TABLE_ENTRY(TIMELINE_BEAT)
    TABLE_ENTRY(SOUND_PLAYED)
    TABLE_ENTRY(SOUND_STOPPED)
    TABLE_ENTRY(REAL_TO_VIRTUAL)
    TABLE_ENTRY(VIRTUAL_TO_REAL)
    TABLE_ENTRY(START_EVENT_COMMAND)
    TABLE_ENTRY(NESTED_TIMELINE_BEAT)
    TABLE_ENTRY(ALL)
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
    TABLE_CREATE(FMOD_DEBUG_FLAGS, "DEBUG_FLAGS");
    TABLE_CREATE(FMOD_INITFLAGS, "INIT");
    TABLE_CREATE(FMOD_TIMEUNIT, "TIMEUNIT");
    TABLE_CREATE(FMOD_MODE, "MODE");
    TABLE_CREATE(FMOD_DEBUG_MODE, "DEBUG_MODE");
    TABLE_CREATE(FMOD_SPEAKERMODE, "SPEAKERMODE");
    TABLE_CREATE(FMOD_CHANNELORDER, "CHANNELORDER");
    TABLE_CREATE(FMOD_SOUND_TYPE, "SOUND_TYPE");
    TABLE_CREATE(FMOD_SOUND_FORMAT, "SOUND_FORMAT");

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

    TABLE_CREATE(FMOD_STUDIO_INITFLAGS, "INIT");
    TABLE_CREATE(FMOD_STUDIO_EVENT_CALLBACK_TYPE, "EVENT_CALLBACK");
    TABLE_CREATE(FMOD_STUDIO_PARAMETER_FLAGS, "PARAMETER_FLAGS");
    TABLE_CREATE(FMOD_STUDIO_LOAD_BANK_FLAGS, "LOAD_BANK");
    TABLE_CREATE(FMOD_STUDIO_PARAMETER_TYPE, "PARAMETER_TYPE");
    TABLE_CREATE(FMOD_STUDIO_STOP_MODE, "STOP");

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
