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

#define SELF_TYPE FMOD_CHANNELGROUP

static int METHOD_NAME(getSystemObject)(lua_State *L)
{
    GET_SELF;

    FMOD_SYSTEM *system = NULL;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetSystemObject(self, &system));

    PUSH_HANDLE(L, FMOD_SYSTEM, system);

    return 1;
}

static int METHOD_NAME(stop)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_ChannelGroup_Stop(self));
}

static int METHOD_NAME(setPaused)(lua_State *L)
{
    GET_SELF;

    int paused = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetPaused(self, paused));
}

static int METHOD_NAME(getPaused)(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL paused = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetPaused(self, &paused));

    lua_pushboolean(L, paused);

    return 1;
}

static int METHOD_NAME(setVolume)(lua_State *L)
{
    GET_SELF;

    float volume = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetVolume(self, volume));
}

static int METHOD_NAME(getVolume)(lua_State *L)
{
    GET_SELF;

    float volume = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetVolume(self, &volume));

    lua_pushnumber(L, volume);

    return 1;
}

static int METHOD_NAME(setVolumeRamp)(lua_State *L)
{
    GET_SELF;

    int ramp = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetVolumeRamp(self, ramp));
}

static int METHOD_NAME(getVolumeRamp)(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL ramp = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetVolumeRamp(self, &ramp));

    lua_pushboolean(L, ramp);

    return 1;
}

static int METHOD_NAME(getAudibility)(lua_State *L)
{
    GET_SELF;

    float audibility = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetVolume(self, &audibility));

    lua_pushnumber(L, audibility);

    return 1;
}

static int METHOD_NAME(setPitch)(lua_State *L)
{
    GET_SELF;

    float pitch = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetPitch(self, pitch));
}

static int METHOD_NAME(getPitch)(lua_State *L)
{
    GET_SELF;

    float pitch = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetPitch(self, &pitch));

    lua_pushnumber(L, pitch);

    return 1;
}

static int METHOD_NAME(setMute)(lua_State *L)
{
    GET_SELF;

    int mute = lua_toboolean(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetMute(self, mute));
}

static int METHOD_NAME(getMute)(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL mute = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetMute(self, &mute));

    lua_pushboolean(L, mute);

    return 1;
}

static int METHOD_NAME(setReverbProperties)(lua_State *L)
{
    GET_SELF;

    int instance = luaL_checkinteger(L, 2);
    float wet = (float)luaL_checknumber(L, 3);

    RETURN_STATUS(FMOD_ChannelGroup_SetReverbProperties(self, instance, wet));
}

static int METHOD_NAME(getReverbProperties)(lua_State *L)
{
    GET_SELF;

    int instance = luaL_checkinteger(L, 2);

    float wet = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetReverbProperties(self, instance, &wet));

    lua_pushnumber(L, wet);

    return 1;
}

static int METHOD_NAME(setLowPassGain)(lua_State *L)
{
    GET_SELF;

    float gain = (float)luaL_checknumber(L, 2);

    RETURN_STATUS(FMOD_ChannelGroup_SetLowPassGain(self, gain));
}

static int METHOD_NAME(getLowPassGain)(lua_State *L)
{
    GET_SELF;

    float gain = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetLowPassGain(self, &gain));

    lua_pushnumber(L, gain);

    return 1;
}

static int METHOD_NAME(setMode)(lua_State *L)
{
    GET_SELF;

    FMOD_MODE mode = CHECK_CONSTANT(L, 2, FMOD_MODE);

    RETURN_STATUS(FMOD_ChannelGroup_SetMode(self, mode));
}

static int METHOD_NAME(getMode)(lua_State *L)
{
    GET_SELF;

    FMOD_MODE mode = FMOD_DEFAULT;
    RETURN_IF_ERROR(FMOD_ChannelGroup_GetMode(self, &mode));

    PUSH_CONSTANT(L, FMOD_MODE, mode);

    return 1;
}

static int METHOD_NAME(isPlaying)(lua_State *L)
{
    GET_SELF;

    FMOD_BOOL playing = 0;
    RETURN_IF_ERROR(FMOD_ChannelGroup_IsPlaying(self, &playing));

    lua_pushboolean(L, playing);

    return 1;
}

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(getSystemObject)
    METHODS_TABLE_ENTRY(stop)
    METHODS_TABLE_ENTRY(setPaused)
    METHODS_TABLE_ENTRY(getPaused)
    METHODS_TABLE_ENTRY(setVolume)
    METHODS_TABLE_ENTRY(getVolume)
    METHODS_TABLE_ENTRY(setVolumeRamp)
    METHODS_TABLE_ENTRY(getVolumeRamp)
    METHODS_TABLE_ENTRY(getAudibility)
    METHODS_TABLE_ENTRY(setPitch)
    METHODS_TABLE_ENTRY(getPitch)
    METHODS_TABLE_ENTRY(setMute)
    METHODS_TABLE_ENTRY(getMute)
    METHODS_TABLE_ENTRY(setReverbProperties)
    METHODS_TABLE_ENTRY(getReverbProperties)
    METHODS_TABLE_ENTRY(setLowPassGain)
    METHODS_TABLE_ENTRY(getLowPassGain)
    METHODS_TABLE_ENTRY(setMode)
    METHODS_TABLE_ENTRY(getMode)
#if 0
    METHODS_TABLE_ENTRY(setCallback)
#endif
    METHODS_TABLE_ENTRY(isPlaying)
#if 0
    METHODS_TABLE_ENTRY(setPan)
    METHODS_TABLE_ENTRY(setMixLevelsOutput)
    METHODS_TABLE_ENTRY(setMixLevelsInput)
    METHODS_TABLE_ENTRY(setMixMatrix)
    METHODS_TABLE_ENTRY(getMixMatrix)
    METHODS_TABLE_ENTRY(getDSPClock)
    METHODS_TABLE_ENTRY(setDelay)
    METHODS_TABLE_ENTRY(getDelay)
    METHODS_TABLE_ENTRY(addFadePoint)
    METHODS_TABLE_ENTRY(setFadePointRamp)
    METHODS_TABLE_ENTRY(removeFadePoints)
    METHODS_TABLE_ENTRY(getFadePoints)
    METHODS_TABLE_ENTRY(getDSP)
    METHODS_TABLE_ENTRY(addDSP)
    METHODS_TABLE_ENTRY(removeDSP)
    METHODS_TABLE_ENTRY(getNumDSPs)
    METHODS_TABLE_ENTRY(setDSPIndex)
    METHODS_TABLE_ENTRY(getDSPIndex)
    METHODS_TABLE_ENTRY(set3DAttributes)
    METHODS_TABLE_ENTRY(get3DAttributes)
    METHODS_TABLE_ENTRY(set3DMinMaxDistance)
    METHODS_TABLE_ENTRY(get3DMinMaxDistance)
    METHODS_TABLE_ENTRY(set3DConeSettings)
    METHODS_TABLE_ENTRY(get3DConeSettings)
    METHODS_TABLE_ENTRY(set3DConeOrientation)
    METHODS_TABLE_ENTRY(get3DConeOrientation)
    METHODS_TABLE_ENTRY(set3DCustomRolloff)
    METHODS_TABLE_ENTRY(get3DCustomRolloff)
    METHODS_TABLE_ENTRY(set3DOcclusion)
    METHODS_TABLE_ENTRY(get3DOcclusion)
    METHODS_TABLE_ENTRY(set3DSpread)
    METHODS_TABLE_ENTRY(get3DSpread)
    METHODS_TABLE_ENTRY(set3DLevel)
    METHODS_TABLE_ENTRY(get3DLevel)
    METHODS_TABLE_ENTRY(set3DDopplerLevel)
    METHODS_TABLE_ENTRY(get3DDopplerLevel)
    METHODS_TABLE_ENTRY(set3DDistanceFilter)
    METHODS_TABLE_ENTRY(get3DDistanceFilter)
    METHODS_TABLE_ENTRY(setUserData)
    METHODS_TABLE_ENTRY(getUserData)
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(addGroup)
    METHODS_TABLE_ENTRY(getNumGroups)
    METHODS_TABLE_ENTRY(getGroup)
    METHODS_TABLE_ENTRY(getParentGroup)
    METHODS_TABLE_ENTRY(getName)
    METHODS_TABLE_ENTRY(getNumChannels)
    METHODS_TABLE_ENTRY(getChannel)
#endif
METHODS_TABLE_END
