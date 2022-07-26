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

#define SELF_TYPE FMOD_STUDIO_EVENTINSTANCE

static int set3DAttributes(lua_State *L)
{
    GET_SELF;

    FMOD_3D_ATTRIBUTES *attributes = CHECK_STRUCT(L, 2, FMOD_3D_ATTRIBUTES);

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

    int mode = CHECK_CONSTANT(L, 2, FMOD_STUDIO_STOP_MODE);

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

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
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

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(set3DAttributes)
    METHODS_TABLE_ENTRY(setPaused)
    METHODS_TABLE_ENTRY(start)
    METHODS_TABLE_ENTRY(stop)
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(setParameterByID)
    METHODS_TABLE_ENTRY(setParameterByName)
    METHODS_TABLE_ENTRY(setCallback)
    METHODS_TABLE_ENTRY(getUserData)
    METHODS_TABLE_ENTRY(setUserData)
METHODS_TABLE_END
