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

#include "callbacks.h"
#include "common.h"
#include <stdlib.h>

#define SELF_TYPE FMOD_STUDIO_EVENTINSTANCE

static int isValid(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL valid = FMOD_Studio_EventInstance_IsValid(self);

    lua_pushboolean(L, valid);

    return 1;
}

static int getDescription(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetDescription(self, &description));

    PUSH_HANDLE(L, FMOD_STUDIO_EVENTDESCRIPTION, description);

    return 1;
}

static int getVolume(lua_State *L)
{
    GET_SELF;

    float volume = 0;
    float finalvolume = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetVolume(self, &volume, &finalvolume));

    lua_pushnumber(L, volume);
    lua_pushnumber(L, finalvolume);

    return 2;
}

static int setVolume(lua_State *L)
{
    GET_SELF;

    float volume = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetVolume(self, volume));
}

static int getPitch(lua_State *L)
{
    GET_SELF;

    float pitch = 0;
    float finalpitch = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetPitch(self, &pitch, &finalpitch));

    lua_pushnumber(L, pitch);
    lua_pushnumber(L, finalpitch);

    return 2;
}

static int setPitch(lua_State *L)
{
    GET_SELF;

    float pitch = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetPitch(self, pitch));
}

static int get3DAttributes(lua_State *L)
{
    GET_SELF;

    FMOD_3D_ATTRIBUTES attributes;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Get3DAttributes(self, &attributes));

    PUSH_STRUCT(L, FMOD_3D_ATTRIBUTES, attributes);

    return 1;
}

static int set3DAttributes(lua_State *L)
{
    GET_SELF;

    FMOD_3D_ATTRIBUTES *attributes = CHECK_STRUCT(L, 2, FMOD_3D_ATTRIBUTES);

    RETURN_STATUS(FMOD_Studio_EventInstance_Set3DAttributes(self, attributes));
}

static int getListenerMask(lua_State *L)
{
    GET_SELF;

    unsigned int mask = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetListenerMask(self, &mask));

    lua_pushinteger(L, mask);

    return 1;
}

static int setListenerMask(lua_State *L)
{
    GET_SELF;

    unsigned int mask = lua_tointeger(L, 2);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetListenerMask(self, mask));
}

static int getProperty(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_EVENT_PROPERTY property = CHECK_CONSTANT(L, 2, FMOD_STUDIO_EVENT_PROPERTY);

    float value = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetProperty(self, property, &value));

    lua_pushnumber(L, value);

    return 1;
}

static int setProperty(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_EVENT_PROPERTY property = CHECK_CONSTANT(L, 2, FMOD_STUDIO_EVENT_PROPERTY);
    float value = (float)luaL_checknumber(L, 3);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetProperty(self, property, value));
}

static int getReverbLevel(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkinteger(L, 2);

    float level = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetReverbLevel(self, index, &level));

    lua_pushnumber(L, level);

    return 1;
}

static int setReverbLevel(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkinteger(L, 2);
    float level = (float)luaL_checknumber(L, 3);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetReverbLevel(self, index, level));
}

static int getPaused(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL paused = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetPaused(self, &paused));

    lua_pushboolean(L, paused);

    return 1;
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

    int mode = CHECK_CONSTANT(L, 2, FMOD_STUDIO_STOP_MODE);

    RETURN_STATUS(FMOD_Studio_EventInstance_Stop(self, mode));
}

static int getTimelinePosition(lua_State *L)
{
    GET_SELF;

    int position = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetTimelinePosition(self, &position));

    lua_pushinteger(L, position);

    return 1;
}

static int setTimelinePosition(lua_State *L)
{
    GET_SELF;

    int position = luaL_checkinteger(L, 2);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetTimelinePosition(self, position));
}

static int getPlaybackState(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PLAYBACK_STATE state;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetPlaybackState(self, &state));

    PUSH_CONSTANT(L, FMOD_STUDIO_PLAYBACK_STATE, state);

    return 1;
}

static int getChannelGroup(lua_State *L)
{
    GET_SELF;

    FMOD_CHANNELGROUP *group = NULL;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetChannelGroup(self, &group));

    PUSH_HANDLE(L, FMOD_CHANNELGROUP, group);

    return 1;
}

static int getMinMaxDistance(lua_State *L)
{
    GET_SELF;

    float min = 0;
    float max = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetMinMaxDistance(self, &min, &max));

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

static int release(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventInstance_Release(self));
}

static int isVirtual(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL virtualstate = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_IsVirtual(self, &virtualstate));

    lua_pushboolean(L, virtualstate);

    return 1;
}

static int getParameterByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    float value = 0;
    float finalvalue = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetParameterByName(self, name, &value, &finalvalue));

    lua_pushnumber(L, value);
    lua_pushnumber(L, finalvalue);

    return 2;
}

static int setParameterByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByName(self, name, value, ignoreseekspeed));
}

static int setParameterByNameWithLabel(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    const char *label = luaL_checkstring(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByNameWithLabel(self, name, label, ignoreseekspeed));
}

static int getParameterByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);

    float value = 0;
    float finalvalue = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetParameterByID(self, *id, &value, &finalvalue));

    lua_pushnumber(L, value);
    lua_pushnumber(L, finalvalue);

    return 2;
}

static int setParameterByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByID(self, *id, value, ignoreseekspeed));
}

static int setParameterByIDWithLabel(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    const char *label = luaL_checkstring(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetParameterByIDWithLabel(self, *id, label, ignoreseekspeed));
}

static int setParametersByIDs(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *ids = NULL;
    float *values = NULL;
    int count = 0;
    getParameterIDsAndValues(L, 2, 3, &ids, &values, &count);

    int ignoreseekspeed = lua_toboolean(L, 4);

#define CLEANUP \
    do { \
        free(values); \
        free(ids); \
    } while(0)

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_SetParametersByIDs(self, ids, values, count, ignoreseekspeed), CLEANUP; );

    CLEANUP;

#undef CLEANUP

    RETURN_STATUS(FMOD_OK);
}

static int keyOff(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_EventInstance_KeyOff(self));
}

static int setCallback(lua_State *L)
{
    GET_SELF;

    int reference = callbackPrepare(L, 2, self);

    RETURN_STATUS(FMOD_Studio_EventInstance_SetCallback(self, eventCallback, FMOD_STUDIO_EVENT_CALLBACK_ALL));

    /*  TODO
        * lock the callback lua_State mutex
        * affirm the callback lua_State
        * copy the function to the callback lua_State (if it's not already there):
            * lua_dump the function into a buffer (or call string.dump)
            * lua_load it into the callback lua_State (or call loadstring)
        * affirm the event instance's callback helper table (in the callback lua_State)
            * the event instance's userdata holds a reference to the callback helper table
            * or it might be better to have a callback table and a userdata table,
              both indexed by FMOD handles (EventInstance or EventDescription)
                * need to copy userdata when creating instances, but avoid setting
                  userdata on an instance also modifying the description
        * store the copied function in the event instance's callback helper table
        * set the event instance's callback to a function that:
            * locks the callback lua_State mutex
            * gets the function from the event instance's callback helper table
            * calls the function with the userdata from the event instance's callback helper table
    */
}

static int getUserData(lua_State *L)
{
    GET_SELF;

    return callbacks_getUserData(L, self);
}

static int setUserData(lua_State *L)
{
    GET_SELF;

    callbacks_checkUserData(L, 2);
    callbacks_setUserData(L, 2, self);

    /* RETURN_STATUS(FMOD_Studio_EventInstance_SetUserData(self, something)); */

    RETURN_STATUS(FMOD_OK);
}

static int getCPUUsage(lua_State *L)
{
    GET_SELF;

    unsigned int exclusive = 0;
    unsigned int inclusive = 0;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetCPUUsage(self, &exclusive, &inclusive));

    lua_pushinteger(L, exclusive);
    lua_pushinteger(L, inclusive);

    return 2;
}

static int getMemoryUsage(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_MEMORY_USAGE usage;
    RETURN_IF_ERROR(FMOD_Studio_EventInstance_GetMemoryUsage(self, &usage));

    PUSH_STRUCT(L, FMOD_STUDIO_MEMORY_USAGE, usage);

    return 1;
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(isValid)
    METHODS_TABLE_ENTRY(getDescription)
    METHODS_TABLE_ENTRY(getVolume)
    METHODS_TABLE_ENTRY(setVolume)
    METHODS_TABLE_ENTRY(getPitch)
    METHODS_TABLE_ENTRY(setPitch)
    METHODS_TABLE_ENTRY(get3DAttributes)
    METHODS_TABLE_ENTRY(set3DAttributes)
    METHODS_TABLE_ENTRY(getListenerMask)
    METHODS_TABLE_ENTRY(setListenerMask)
    METHODS_TABLE_ENTRY(getProperty)
    METHODS_TABLE_ENTRY(setProperty)
    METHODS_TABLE_ENTRY(getReverbLevel)
    METHODS_TABLE_ENTRY(setReverbLevel)
    METHODS_TABLE_ENTRY(getPaused)
    METHODS_TABLE_ENTRY(setPaused)
    METHODS_TABLE_ENTRY(start)
    METHODS_TABLE_ENTRY(stop)
    METHODS_TABLE_ENTRY(getTimelinePosition)
    METHODS_TABLE_ENTRY(setTimelinePosition)
    METHODS_TABLE_ENTRY(getPlaybackState)
    METHODS_TABLE_ENTRY(getChannelGroup)
    METHODS_TABLE_ENTRY(getMinMaxDistance)
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(isVirtual)
    METHODS_TABLE_ENTRY(getParameterByName)
    METHODS_TABLE_ENTRY(setParameterByName)
    METHODS_TABLE_ENTRY(setParameterByNameWithLabel)
    METHODS_TABLE_ENTRY(getParameterByID)
    METHODS_TABLE_ENTRY(setParameterByID)
    METHODS_TABLE_ENTRY(setParameterByIDWithLabel)
    METHODS_TABLE_ENTRY(setParametersByIDs)
    METHODS_TABLE_ENTRY(keyOff)
    METHODS_TABLE_ENTRY(setCallback)
    METHODS_TABLE_ENTRY(getUserData)
    METHODS_TABLE_ENTRY(setUserData)
    METHODS_TABLE_ENTRY(getCPUUsage)
    METHODS_TABLE_ENTRY(getMemoryUsage)
METHODS_TABLE_END
