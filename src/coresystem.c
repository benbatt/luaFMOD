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

static int setSoftwareFormat(lua_State *L)
{
    GET_SELF;

    int samplerate = luaL_checkint(L, 2);
    int speakermode = CHECK_CONSTANT(3, FMOD_SPEAKERMODE);
    int numrawspeakers = luaL_checkint(L, 4);

    REQUIRE_OK(FMOD_System_SetSoftwareFormat(self, samplerate, speakermode, numrawspeakers));

    return 0;
}

static int createSound(lua_State *L)
{
    GET_SELF;

    size_t name_or_data_length = 0;
    const char *name_or_data = luaL_checklstring(L, 2, &name_or_data_length);
    int mode = CHECK_CONSTANT(3, FMOD_MODE);

    FMOD_CREATESOUNDEXINFO *exinfo = FMOD_CREATESOUNDEXINFO_todata(L, 4, 0);
    if (exinfo) {
        exinfo->cbsize = sizeof(*exinfo);
    }

    FMOD_SOUND *sound = NULL;
    RETURN_IF_ERROR(FMOD_System_CreateSound(self, name_or_data, mode, exinfo, &sound));

    CREATE_USERDATA(FMOD_SOUND, sound);

    return 1;
}

static int playSound(lua_State *L)
{
    GET_SELF;

    FMOD_SOUND *sound = *((FMOD_SOUND**)luaL_checkudata(L, 2, FMOD_SOUND_METATABLE));
    int paused = lua_toboolean(L, 3);

    FMOD_CHANNEL *channel = NULL;
    // TODO channelgroup
    RETURN_IF_ERROR(FMOD_System_PlaySound(self, sound, NULL, paused, &channel));

    CREATE_USERDATA(FMOD_CHANNEL, channel);

    return 1;
}

FUNCTION_TABLE_BEGIN(CoreSystemMethods)
    FUNCTION_TABLE_ENTRY(setSoftwareFormat)
    FUNCTION_TABLE_ENTRY(createSound)
    FUNCTION_TABLE_ENTRY(playSound)
FUNCTION_TABLE_END
