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

#define SELF_TYPE FMOD_STUDIO_BANK

static int METHOD_NAME(isValid)(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL valid = FMOD_Studio_Bank_IsValid(self);

    lua_pushboolean(L, valid);

    return 1;
}

static int METHOD_NAME(getID)(lua_State *L)
{
    GET_SELF;

    FMOD_GUID id;

    RETURN_IF_ERROR(FMOD_Studio_Bank_GetID(self, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

static int METHOD_NAME(getPath)(lua_State *L)
{
    GET_SELF;

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetPath(self, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    RETURN_IF_ERROR(FMOD_Studio_Bank_GetPath(self, path, size, &size),
        STACKBUFFER_RELEASE(path););

    lua_pushstring(L, path);

    STACKBUFFER_RELEASE(path);

    return 1;
}

static int METHOD_NAME(unload)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_Bank_Unload(self));
}

static int METHOD_NAME(loadSampleData)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_Bank_LoadSampleData(self));
}

static int METHOD_NAME(unloadSampleData)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_Bank_UnloadSampleData(self));
}

static int METHOD_NAME(getLoadingState)(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_LOADING_STATE state;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetLoadingState(self, &state));

    PUSH_CONSTANT(L, FMOD_STUDIO_LOADING_STATE, state);

    return 1;
}

static int METHOD_NAME(getSampleLoadingState)(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_LOADING_STATE state;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetSampleLoadingState(self, &state));

    PUSH_CONSTANT(L, FMOD_STUDIO_LOADING_STATE, state);

    return 1;
}

static int METHOD_NAME(getStringCount)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetStringCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int METHOD_NAME(getStringInfo)(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkinteger(L, 2);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetStringInfo(self, index, NULL, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    FMOD_GUID id;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetStringInfo(self, index, &id, path, size, &size),
        STACKBUFFER_RELEASE(path););

    PUSH_STRUCT(L, FMOD_GUID, id);
    lua_pushstring(L, path);

    STACKBUFFER_RELEASE(path);

    return 2;
}

static int METHOD_NAME(getEventCount)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetEventCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int METHOD_NAME(getEventList)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetEventCount(self, &count));

    STACKBUFFER_CREATE(FMOD_STUDIO_EVENTDESCRIPTION*, array, count);

    RETURN_IF_ERROR(FMOD_Studio_Bank_GetEventList(self, array, count, NULL),
        STACKBUFFER_RELEASE(array););

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i)
    {
        PUSH_HANDLE(L, FMOD_STUDIO_EVENTDESCRIPTION, array[i]);
        lua_rawseti(L, -2, i + 1);
    }

    STACKBUFFER_RELEASE(array);

    return 1;
}

static int METHOD_NAME(getBusCount)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetBusCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int METHOD_NAME(getBusList)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetBusCount(self, &count));

    STACKBUFFER_CREATE(FMOD_STUDIO_BUS*, array, count);

    RETURN_IF_ERROR(FMOD_Studio_Bank_GetBusList(self, array, count, NULL),
        STACKBUFFER_RELEASE(array););

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i)
    {
        PUSH_HANDLE(L, FMOD_STUDIO_BUS, array[i]);
        lua_rawseti(L, -2, i + 1);
    }

    STACKBUFFER_RELEASE(array);

    return 1;
}

static int METHOD_NAME(getVCACount)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetVCACount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int METHOD_NAME(getVCAList)(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_Bank_GetVCACount(self, &count));

    STACKBUFFER_CREATE(FMOD_STUDIO_VCA*, array, count);

    RETURN_IF_ERROR(FMOD_Studio_Bank_GetVCAList(self, array, count, NULL),
        STACKBUFFER_RELEASE(array););

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i)
    {
        PUSH_HANDLE(L, FMOD_STUDIO_VCA, array[i]);
        lua_rawseti(L, -2, i + 1);
    }

    STACKBUFFER_RELEASE(array);

    return 1;
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(isValid)
    METHODS_TABLE_ENTRY(getID)
    METHODS_TABLE_ENTRY(getPath)
    METHODS_TABLE_ENTRY(unload)
    METHODS_TABLE_ENTRY(loadSampleData)
    METHODS_TABLE_ENTRY(unloadSampleData)
    METHODS_TABLE_ENTRY(getLoadingState)
    METHODS_TABLE_ENTRY(getSampleLoadingState)
    METHODS_TABLE_ENTRY(getStringCount)
    METHODS_TABLE_ENTRY(getStringInfo)
    METHODS_TABLE_ENTRY(getEventCount)
    METHODS_TABLE_ENTRY(getEventList)
    METHODS_TABLE_ENTRY(getBusCount)
    METHODS_TABLE_ENTRY(getBusList)
    METHODS_TABLE_ENTRY(getVCACount)
    METHODS_TABLE_ENTRY(getVCAList)
METHODS_TABLE_END
