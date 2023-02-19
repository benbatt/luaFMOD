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

    lua_pushstring(L, path);

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

static int getParameterLabelByIndex(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkint(L, 2);
    int labelindex = luaL_checkint(L, 3);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByIndex(self, index, labelindex, NULL, 0, &size));

    STACKBUFFER_CREATE(char, label, size);

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByIndex(self, index, labelindex, label, size, &size),
        STACKBUFFER_RELEASE(label););

    lua_pushstring(L, label);

    STACKBUFFER_RELEASE(label);

    return 1;
}

static int getParameterLabelByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    int labelindex = luaL_checkint(L, 3);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByName(self, name, labelindex, NULL, 0, &size));

    STACKBUFFER_CREATE(char, label, size);

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByName(self, name, labelindex, label, size, &size),
        STACKBUFFER_RELEASE(label););

    lua_pushstring(L, label);

    STACKBUFFER_RELEASE(label);

    return 1;
}

static int getParameterLabelByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    int labelindex = luaL_checkint(L, 3);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByID(self, *id, labelindex, NULL, 0, &size));

    STACKBUFFER_CREATE(char, label, size);

    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetParameterLabelByID(self, *id, labelindex, label, size, &size),
        STACKBUFFER_RELEASE(label););

    lua_pushstring(L, label);

    STACKBUFFER_RELEASE(label);

    return 1;
}

static int getUserPropertyCount(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetUserPropertyCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int getUserPropertyByIndex(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkint(L, 2);

    FMOD_STUDIO_USER_PROPERTY property;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetUserPropertyByIndex(self, index, &property));

    PUSH_STRUCT(L, FMOD_STUDIO_USER_PROPERTY, property);

    return 1;
}

static int getUserProperty(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    FMOD_STUDIO_USER_PROPERTY property;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetUserProperty(self, name, &property));

    PUSH_STRUCT(L, FMOD_STUDIO_USER_PROPERTY, property);

    return 1;
}

static int getLength(lua_State *L)
{
    GET_SELF;

    int length = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetLength(self, &length));

    lua_pushnumber(L, length);

    return 1;
}

static int getMinMaxDistance(lua_State *L)
{
    GET_SELF;

    float min = 0;
    float max = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetMinMaxDistance(self, &min, &max));

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

static int getSoundSize(lua_State *L)
{
    GET_SELF;

    float size = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_GetSoundSize(self, &size));

    lua_pushnumber(L, size);

    return 1;
}

static int isSnapshot(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL snapshot = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_IsSnapshot(self, &snapshot));

    lua_pushboolean(L, snapshot);

    return 1;
}

static int isOneshot(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL oneshot = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_IsOneshot(self, &oneshot));

    lua_pushboolean(L, oneshot);

    return 1;
}

static int isStream(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL isStream = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_IsStream(self, &isStream));

    lua_pushboolean(L, isStream);

    return 1;
}

static int is3D(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL is3D = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_Is3D(self, &is3D));

    lua_pushboolean(L, is3D);

    return 1;
}

static int isDopplerEnabled(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL doppler = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_IsDopplerEnabled(self, &doppler));

    lua_pushboolean(L, doppler);

    return 1;
}

static int hasSustainPoint(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL sustainpoint = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_HasSustainPoint(self, &sustainpoint));

    lua_pushboolean(L, sustainpoint);

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
    METHODS_TABLE_ENTRY(getParameterLabelByIndex)
    METHODS_TABLE_ENTRY(getParameterLabelByName)
    METHODS_TABLE_ENTRY(getParameterLabelByID)
    METHODS_TABLE_ENTRY(getUserPropertyCount)
    METHODS_TABLE_ENTRY(getUserPropertyByIndex)
    METHODS_TABLE_ENTRY(getUserProperty)
    METHODS_TABLE_ENTRY(getLength)
    METHODS_TABLE_ENTRY(getMinMaxDistance)
    METHODS_TABLE_ENTRY(getSoundSize)
    METHODS_TABLE_ENTRY(isSnapshot)
    METHODS_TABLE_ENTRY(isOneshot)
    METHODS_TABLE_ENTRY(isStream)
    METHODS_TABLE_ENTRY(is3D)
    METHODS_TABLE_ENTRY(isDopplerEnabled)
    METHODS_TABLE_ENTRY(hasSustainPoint)
    METHODS_TABLE_ENTRY(createInstance)
    METHODS_TABLE_ENTRY(loadSampleData)
METHODS_TABLE_END
