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

static int isValid(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL valid = FMOD_Studio_EventDescription_IsValid(self);

    lua_pushboolean(L, valid);

    return 1;
}

static int getID(lua_State *L)
{
    GET_SELF;

    FMOD_GUID id;

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetID(self, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

static int getPath(lua_State *L)
{
    GET_SELF;

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetPath(self, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetPath(self, path, size, &size),
        STACKBUFFER_RELEASE(path););

    lua_pushlstring(L, path, size);

    STACKBUFFER_RELEASE(path);

    return 1;
}

static int getParameterDescriptionCount(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterDescriptionCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int getParameterDescriptionByIndex(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkint(L, 2);

    FMOD_STUDIO_PARAMETER_DESCRIPTION description;

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterDescriptionByIndex(self, index, &description));

    PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, description);

    return 1;
}

static int getParameterDescriptionByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    FMOD_STUDIO_PARAMETER_DESCRIPTION description;

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterDescriptionByName(self, name, &description));

    PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, description);

    return 1;
}

static int getParameterDescriptionByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);

    FMOD_STUDIO_PARAMETER_DESCRIPTION description;

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterDescriptionByID(self, *id, &description));

    PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, description);

    return 1;
}

static int createInstance(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_CreateInstance(self, &instance));

    PUSH_HANDLE(L, FMOD_STUDIO_EVENTINSTANCE, instance);

    return 1;
}

static int loadSampleData(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventDescription_LoadSampleData(self));
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(isValid)
    METHODS_TABLE_ENTRY(getID)
    METHODS_TABLE_ENTRY(getPath)
    METHODS_TABLE_ENTRY(getParameterDescriptionCount)
    METHODS_TABLE_ENTRY(getParameterDescriptionByIndex)
    METHODS_TABLE_ENTRY(getParameterDescriptionByName)
    METHODS_TABLE_ENTRY(getParameterDescriptionByID)
    METHODS_TABLE_ENTRY(createInstance)
    METHODS_TABLE_ENTRY(loadSampleData)
METHODS_TABLE_END
