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
#define FMOD_PREFIX FMOD_ChannelGroup_

#include "templates.h"

static int GET_FMOD_CHANNEL_indexed(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, int, FMOD_CHANNEL **))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);

  FMOD_CHANNEL *handle;
  RETURN_IF_ERROR(getter(self, index, &handle));

  PUSH_HANDLE(L, FMOD_CHANNEL, handle);

  return 1;
}

static int GET_FMOD_CHANNELGROUP_indexed(lua_State *L,
  FMOD_RESULT F_API (*getter)(SELF_TYPE *, int, FMOD_CHANNELGROUP **))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);

  FMOD_CHANNELGROUP *handle;
  RETURN_IF_ERROR(getter(self, index, &handle));

  PUSH_HANDLE(L, FMOD_CHANNELGROUP, handle);

  return 1;
}

GET_HANDLE(FMOD_SYSTEM, SystemObject)

static int METHOD_NAME(stop)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_ChannelGroup_Stop(self));
}

PROPERTY(FMOD_BOOL, Paused)
PROPERTY(float, Volume)
PROPERTY(FMOD_BOOL, VolumeRamp)
GET(float, Audibility)
PROPERTY(float, Pitch)
PROPERTY(FMOD_BOOL, Mute)
PROPERTY_FLOAT_INDEXED(ReverbProperties)
PROPERTY(float, LowPassGain)
PROPERTY_CONSTANT(FMOD_MODE, Mode)
GET_CUSTOM(FMOD_BOOL, isPlaying, IsPlaying)
SET(float, Pan)
SET_MULTI(MixLevelsOutput, float, float, float, float, float, float, float, float)
HANDLE_LIST(FMOD_DSP, DSP)
PROPERTY_MULTI(3DAttributes, FMOD_VECTOR, FMOD_VECTOR)
PROPERTY_MULTI(3DMinMaxDistance, float, float)
PROPERTY_MULTI(3DConeSettings, float, float, float)
PROPERTY(FMOD_VECTOR, 3DConeOrientation)
PROPERTY_MULTI(3DOcclusion, float, float)
PROPERTY(float, 3DSpread)
PROPERTY(float, 3DLevel)
PROPERTY(float, 3DDopplerLevel)
PROPERTY_MULTI(3DDistanceFilter, FMOD_BOOL, float, float)
HANDLE_LIST(FMOD_CHANNELGROUP, Group)
HANDLE_LIST(FMOD_CHANNEL, Channel)

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
  METHODS_TABLE_ENTRY(setPan)
  METHODS_TABLE_ENTRY(setMixLevelsOutput)
#if 0
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
#endif
  METHODS_TABLE_ENTRY(getDSP)
#if 0
  METHODS_TABLE_ENTRY(addDSP)
  METHODS_TABLE_ENTRY(removeDSP)
#endif
  METHODS_TABLE_ENTRY(getNumDSPs)
#if 0
  METHODS_TABLE_ENTRY(setDSPIndex)
  METHODS_TABLE_ENTRY(getDSPIndex)
#endif
  METHODS_TABLE_ENTRY(set3DAttributes)
  METHODS_TABLE_ENTRY(get3DAttributes)
  METHODS_TABLE_ENTRY(set3DMinMaxDistance)
  METHODS_TABLE_ENTRY(get3DMinMaxDistance)
  METHODS_TABLE_ENTRY(set3DConeSettings)
  METHODS_TABLE_ENTRY(get3DConeSettings)
  METHODS_TABLE_ENTRY(set3DConeOrientation)
  METHODS_TABLE_ENTRY(get3DConeOrientation)
#if 0
  METHODS_TABLE_ENTRY(set3DCustomRolloff)
  METHODS_TABLE_ENTRY(get3DCustomRolloff)
#endif
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
#if 0
  METHODS_TABLE_ENTRY(setUserData)
  METHODS_TABLE_ENTRY(getUserData)
  METHODS_TABLE_ENTRY(release)
  METHODS_TABLE_ENTRY(addGroup)
#endif
  METHODS_TABLE_ENTRY(getNumGroups)
  METHODS_TABLE_ENTRY(getGroup)
#if 0
  METHODS_TABLE_ENTRY(getParentGroup)
  METHODS_TABLE_ENTRY(getName)
#endif
  METHODS_TABLE_ENTRY(getNumChannels)
  METHODS_TABLE_ENTRY(getChannel)
METHODS_TABLE_END
