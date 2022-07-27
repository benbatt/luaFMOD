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

static int set3DAttributes(lua_State *L)
{
    GET_SELF(FMOD_STUDIO_EVENTINSTANCE);

    FMOD_3D_ATTRIBUTES *attributes = FMOD_3D_ATTRIBUTES_todata(L, 2, STRUCT_REQUIRED);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Set3DAttributes(self, attributes));

    RETURN_TRUE;
}

static int start(lua_State *L)
{
    GET_SELF(FMOD_STUDIO_EVENTINSTANCE);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Start(self));

    RETURN_TRUE;
}

static int stop(lua_State *L)
{
    GET_SELF(FMOD_STUDIO_EVENTINSTANCE);

    int mode = CHECK_CONSTANT(2, FMOD_STUDIO_STOP_MODE);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Stop(self, mode));

    RETURN_TRUE;
}

static int release(lua_State *L)
{
    GET_SELF(FMOD_STUDIO_EVENTINSTANCE);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Release(self));

    RETURN_TRUE;
}

static int setParameterByName(lua_State *L)
{
    GET_SELF(FMOD_STUDIO_EVENTINSTANCE);

    const char *name = luaL_checkstring(L, 2);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_SetParameterByName(self, name, value, ignoreseekspeed));

    RETURN_TRUE;
}

FUNCTION_TABLE_BEGIN(EventInstanceMethods)
    FUNCTION_TABLE_ENTRY(set3DAttributes)
    FUNCTION_TABLE_ENTRY(start)
    FUNCTION_TABLE_ENTRY(stop)
    FUNCTION_TABLE_ENTRY(release)
    FUNCTION_TABLE_ENTRY(setParameterByName)
FUNCTION_TABLE_END
