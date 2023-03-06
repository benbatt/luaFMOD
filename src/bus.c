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

#define SELF_TYPE FMOD_STUDIO_BUS

static int isValid(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL valid = FMOD_Studio_Bus_IsValid(self);

    lua_pushboolean(L, valid);

    return 1;
}

static int getID(lua_State *L)
{
    GET_SELF;

    FMOD_GUID id;

    RETURN_IF_ERROR(FMOD_Studio_Bus_GetID(self, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

static int getPath(lua_State *L)
{
    GET_SELF;

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bus_GetPath(self, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    RETURN_IF_ERROR(FMOD_Studio_Bus_GetPath(self, path, size, &size),
        STACKBUFFER_RELEASE(path););

    lua_pushstring(L, path);

    STACKBUFFER_RELEASE(path);

    return 1;
}

static int getVolume(lua_State *L)
{
    GET_SELF;

    float volume = 0;
    float finalvolume = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bus_GetVolume(self, &volume, &finalvolume));

    lua_pushnumber(L, volume);
    lua_pushnumber(L, finalvolume);

    return 2;
}

static int setVolume(lua_State *L)
{
    GET_SELF;

    float volume = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_Studio_Bus_SetVolume(self, volume));
}

static int getPaused(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL paused = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bus_GetPaused(self, &paused));

    lua_pushboolean(L, paused);

    return 1;
}

static int setPaused(lua_State *L)
{
    GET_SELF;

    int paused = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_Studio_Bus_SetPaused(self, paused));
}

static int getMute(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL mute = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bus_GetMute(self, &mute));

    lua_pushboolean(L, mute);

    return 1;
}

static int setMute(lua_State *L)
{
    GET_SELF;

    int mute = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_Studio_Bus_SetMute(self, mute));
}

static int stopAllEvents(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_STOP_MODE mode = CHECK_CONSTANT(L, 2, FMOD_STUDIO_STOP_MODE);

    RETURN_STATUS(FMOD_Studio_Bus_StopAllEvents(self, mode));
}

static int getPortIndex(lua_State *L)
{
    GET_SELF;

    FMOD_PORT_INDEX index = 0xBaadF00dDeadBeef;
    RETURN_IF_ERROR(FMOD_Studio_Bus_GetPortIndex(self, &index));

    lua_pushinteger(L, (unsigned int)(index & 0xFFFFFFFF));
    lua_pushinteger(L, (unsigned int)(index >> 32));

    return 2;
}

static int setPortIndex(lua_State *L)
{
    GET_SELF;

    FMOD_PORT_INDEX low = (unsigned int)luaL_checkinteger(L, 2);
    FMOD_PORT_INDEX high = (unsigned int)luaL_optinteger(L, 3, 0);

    FMOD_PORT_INDEX index = (high << 32) | low;

    RETURN_STATUS(FMOD_Studio_Bus_SetPortIndex(self, index));
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(isValid)
    METHODS_TABLE_ENTRY(getID)
    METHODS_TABLE_ENTRY(getPath)
    METHODS_TABLE_ENTRY(getVolume)
    METHODS_TABLE_ENTRY(setVolume)
    METHODS_TABLE_ENTRY(getPaused)
    METHODS_TABLE_ENTRY(setPaused)
    METHODS_TABLE_ENTRY(getMute)
    METHODS_TABLE_ENTRY(setMute)
    METHODS_TABLE_ENTRY(stopAllEvents)
    METHODS_TABLE_ENTRY(getPortIndex)
    METHODS_TABLE_ENTRY(setPortIndex)
METHODS_TABLE_END
