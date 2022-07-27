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

#define SELF_TYPE FMOD_STUDIO_EVENTDESCRIPTION

static int getParameterDescriptionByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    FMOD_STUDIO_PARAMETER_DESCRIPTION_new(L);
    FMOD_STUDIO_PARAMETER_DESCRIPTION *description = FMOD_STUDIO_PARAMETER_DESCRIPTION_todata(L, -1, STRUCT_REQUIRED);

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterDescriptionByName(self, name, description));

    return 1;
}

static int createInstance(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_CreateInstance(self, &instance));

    CREATE_USERDATA(FMOD_STUDIO_EVENTINSTANCE, instance);

    return 1;
}

static int loadSampleData(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventDescription_LoadSampleData(self));
}

FUNCTION_TABLE_BEGIN(EventDescriptionMethods)
    FUNCTION_TABLE_ENTRY(getParameterDescriptionByName)
    FUNCTION_TABLE_ENTRY(createInstance)
    FUNCTION_TABLE_ENTRY(loadSampleData)
FUNCTION_TABLE_END
