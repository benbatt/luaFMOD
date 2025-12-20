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

#define SELF_TYPE FMOD_SYSTEM

static int METHOD_NAME(create)(lua_State *L)
{
    FMOD_SYSTEM *system = NULL;

    REQUIRE_OK(FMOD_System_Create(&system, FMOD_VERSION));

    PUSH_HANDLE(L, FMOD_SYSTEM, system);

    return 1;
}

static int METHOD_NAME(release)(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_System_Release(self));

    return 0;
}

static int METHOD_NAME(init)(lua_State *L)
{
    GET_SELF;

    int maxchannels = luaL_checkint(L, 2);
    int flags = CHECK_CONSTANT(L, 3, FMOD_INITFLAGS);

    /* TODO extradriverdata? */

    REQUIRE_OK(FMOD_System_Init(self, maxchannels, flags, NULL));

    return 0;
}

static int METHOD_NAME(close)(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_System_Close(self));

    return 0;
}

static int METHOD_NAME(update)(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_System_Update(self));

    return 0;
}

static int METHOD_NAME(setSoftwareFormat)(lua_State *L)
{
    GET_SELF;

    int samplerate = luaL_checkint(L, 2);
    int speakermode = CHECK_CONSTANT(L, 3, FMOD_SPEAKERMODE);
    int numrawspeakers = luaL_checkint(L, 4);

    REQUIRE_OK(FMOD_System_SetSoftwareFormat(self, samplerate, speakermode, numrawspeakers));

    return 0;
}

static int METHOD_NAME(createSound)(lua_State *L)
{
    GET_SELF;

    const char *name_or_data = NULL;

    if (lua_type(L, 2) == LUA_TUSERDATA) {
        name_or_data = *CHECK_STRUCT(L, 2, luaFMOD_Buffer);
    } else {
        name_or_data = luaL_checkstring(L, 2);
    }

    int mode = CHECK_CONSTANT(L, 3, FMOD_MODE);

    FMOD_CREATESOUNDEXINFO *exinfo = OPTIONAL_STRUCT(L, 4, FMOD_CREATESOUNDEXINFO);
    if (exinfo) {
        exinfo->cbsize = sizeof(*exinfo);
    }

    FMOD_SOUND *sound = NULL;
    RETURN_IF_ERROR(FMOD_System_CreateSound(self, name_or_data, mode, exinfo, &sound));

    PUSH_HANDLE(L, FMOD_SOUND, sound);

    return 1;
}

static int METHOD_NAME(playSound)(lua_State *L)
{
    GET_SELF;

    FMOD_SOUND *sound = CHECK_HANDLE(L, 2, FMOD_SOUND);
    int paused = lua_toboolean(L, 3);

    FMOD_CHANNEL *channel = NULL;
    // TODO channelgroup
    RETURN_IF_ERROR(FMOD_System_PlaySound(self, sound, NULL, paused, &channel));

    PUSH_HANDLE(L, FMOD_CHANNEL, channel);

    return 1;
}

static int METHOD_NAME(createChannelGroup)(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_optstring(L, 2, NULL);

    FMOD_CHANNELGROUP *channelgroup = NULL;
    RETURN_IF_ERROR(FMOD_System_CreateChannelGroup(self, name, &channelgroup));

    PUSH_HANDLE(L, FMOD_CHANNELGROUP, channelgroup);

    return 1;
}


static int METHOD_NAME(getMasterChannelGroup)(lua_State *L)
{
    GET_SELF;

    FMOD_CHANNELGROUP *channelgroup = NULL;
    RETURN_IF_ERROR(FMOD_System_GetMasterChannelGroup(self, &channelgroup));

    PUSH_HANDLE(L, FMOD_CHANNELGROUP, channelgroup);

    return 1;
}

FUNCTION_TABLE_BEGIN(SystemStaticFunctions)
    METHODS_TABLE_ENTRY(create)
FUNCTION_TABLE_END

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(setSoftwareFormat)
    METHODS_TABLE_ENTRY(init)
    METHODS_TABLE_ENTRY(close)
    METHODS_TABLE_ENTRY(update)
    METHODS_TABLE_ENTRY(createSound)
    METHODS_TABLE_ENTRY(playSound)
    METHODS_TABLE_ENTRY(createChannelGroup)
    METHODS_TABLE_ENTRY(getMasterChannelGroup)
METHODS_TABLE_END
