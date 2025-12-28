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

GET(SystemObject, (FMOD_SYSTEM, HANDLE))

static int METHOD_NAME(stop)(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_ChannelGroup_Stop(self));
}

PROPERTY(Paused, FMOD_BOOL)
PROPERTY(Volume, float)
PROPERTY(VolumeRamp, FMOD_BOOL)
GET(Audibility, float)
PROPERTY(Pitch, float)
PROPERTY(Mute, FMOD_BOOL)
PROPERTY_INDEXED(ReverbProperties, int, float)
PROPERTY(LowPassGain, float)
PROPERTY(Mode, (FMOD_MODE, CONSTANT))
GET_CUSTOM(isPlaying, FMOD_BOOL, IsPlaying)
SET(Pan, float)
SET_MULTI(MixLevelsOutput, float, float, float, float, float, float, float, float)
PROPERTY_MULTI(Delay, DSP_CLOCK, DSP_CLOCK, FMOD_BOOL)
HANDLE_LIST(DSP, FMOD_DSP)
PROPERTY_INDEXED(DSPIndex, (FMOD_DSP, HANDLE), int)
PROPERTY_MULTI(3DAttributes, (FMOD_VECTOR, STRUCT), (FMOD_VECTOR, STRUCT))
PROPERTY_MULTI(3DMinMaxDistance, float, float)
PROPERTY_MULTI(3DConeSettings, float, float, float)
PROPERTY(3DConeOrientation, FMOD_VECTOR)
PROPERTY_MULTI(3DOcclusion, float, float)
PROPERTY(3DSpread, float)
PROPERTY(3DLevel, float)
PROPERTY(3DDopplerLevel, float)
PROPERTY_MULTI(3DDistanceFilter, FMOD_BOOL, float, float)
HANDLE_LIST(Group, FMOD_CHANNELGROUP)
GET(ParentGroup, (FMOD_CHANNELGROUP, HANDLE))
HANDLE_LIST(Channel, FMOD_CHANNEL)

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
#endif
  METHODS_TABLE_ENTRY(setDelay)
  METHODS_TABLE_ENTRY(getDelay)
#if 0
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
  METHODS_TABLE_ENTRY(getParentGroup)
#if 0
  METHODS_TABLE_ENTRY(getName)
#endif
  METHODS_TABLE_ENTRY(getNumChannels)
  METHODS_TABLE_ENTRY(getChannel)
METHODS_TABLE_END
