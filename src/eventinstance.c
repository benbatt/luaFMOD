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

#define SELF_TYPE FMOD_STUDIO_EVENTINSTANCE

static int set3DAttributes(lua_State *L)
{
    GET_SELF;

    FMOD_3D_ATTRIBUTES *attributes = FMOD_3D_ATTRIBUTES_todata(L, 2, STRUCT_REQUIRED);

    RETURN_STATUS(FMOD_Studio_EventInstance_Set3DAttributes(self, attributes));
}

static int setPaused(lua_State *L)
{
    GET_SELF;

    int paused = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetPaused(self, paused));
}

static int start(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventInstance_Start(self));
}

static int stop(lua_State *L)
{
    GET_SELF;

    int mode = CHECK_CONSTANT(2, FMOD_STUDIO_STOP_MODE);

    RETURN_STATUS(FMOD_Studio_EventInstance_Stop(self, mode));
}

static int release(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventInstance_Release(self));
}

static int setParameterByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = FMOD_STUDIO_PARAMETER_ID_todata(L, 2, STRUCT_REQUIRED);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByID(self, *id, value, ignoreseekspeed));
}

static int setParameterByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByName(self, name, value, ignoreseekspeed));
}

FUNCTION_TABLE_BEGIN(EventInstanceMethods)
    FUNCTION_TABLE_ENTRY(set3DAttributes)
    FUNCTION_TABLE_ENTRY(setPaused)
    FUNCTION_TABLE_ENTRY(start)
    FUNCTION_TABLE_ENTRY(stop)
    FUNCTION_TABLE_ENTRY(release)
    FUNCTION_TABLE_ENTRY(setParameterByID)
    FUNCTION_TABLE_ENTRY(setParameterByName)
FUNCTION_TABLE_END
