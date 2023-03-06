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

#define SELF_TYPE FMOD_STUDIO_VCA

static int isValid(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL valid = FMOD_Studio_VCA_IsValid(self);

    lua_pushboolean(L, valid);

    return 1;
}

static int getID(lua_State *L)
{
    GET_SELF;

    FMOD_GUID id;

    RETURN_IF_ERROR(FMOD_Studio_VCA_GetID(self, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

static int getPath(lua_State *L)
{
    GET_SELF;

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_VCA_GetPath(self, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    RETURN_IF_ERROR(FMOD_Studio_VCA_GetPath(self, path, size, &size),
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
    RETURN_IF_ERROR(FMOD_Studio_VCA_GetVolume(self, &volume, &finalvolume));

    lua_pushnumber(L, volume);
    lua_pushnumber(L, finalvolume);

    return 2;
}

static int setVolume(lua_State *L)
{
    GET_SELF;

    float volume = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_Studio_VCA_SetVolume(self, volume));
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(isValid)
    METHODS_TABLE_ENTRY(getID)
    METHODS_TABLE_ENTRY(getPath)
    METHODS_TABLE_ENTRY(getVolume)
    METHODS_TABLE_ENTRY(setVolume)
METHODS_TABLE_END
