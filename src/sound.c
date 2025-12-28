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

#define SELF_TYPE FMOD_SOUND
#define FMOD_PREFIX FMOD_Sound_

#include "templates.h"

static int GET_FMOD_SOUND_indexed(lua_State *L, FMOD_RESULT F_API (*getter)(SELF_TYPE *, int, FMOD_SOUND **))
{
  GET_SELF;

  int index = luaL_checkinteger(L, 2);

  FMOD_SOUND *handle = NULL;
  RETURN_IF_ERROR(getter(self, index, &handle));

  PUSH_HANDLE(L, FMOD_SOUND, handle);

  return 1;
}

GET(SystemObject, (FMOD_SYSTEM, HANDLE))
HANDLE_LIST(SubSound, FMOD_SOUND)
GET(SubSoundParent, (FMOD_SOUND, HANDLE))
PROPERTY_MULTI(Defaults, float, int)
PROPERTY_MULTI(3DMinMaxDistance, float, float)
PROPERTY_MULTI(3DConeSettings, float, float, float)
PROPERTY(Mode, (FMOD_MODE, CONSTANT))
PROPERTY(LoopCount, int)
GET(MusicNumChannels, int)
PROPERTY_INDEXED(MusicChannelVolume, int, float)
PROPERTY(MusicSpeed, float)

METHODS_TABLE_BEGIN
#if 0
    METHODS_TABLE_ENTRY(release)
#endif
    METHODS_TABLE_ENTRY(getSystemObject)
#if 0
    METHODS_TABLE_ENTRY(lock)
    METHODS_TABLE_ENTRY(unlock)
#endif
    METHODS_TABLE_ENTRY(setDefaults)
    METHODS_TABLE_ENTRY(getDefaults)
    METHODS_TABLE_ENTRY(set3DMinMaxDistance)
    METHODS_TABLE_ENTRY(get3DMinMaxDistance)
    METHODS_TABLE_ENTRY(set3DConeSettings)
    METHODS_TABLE_ENTRY(get3DConeSettings)
#if 0
    METHODS_TABLE_ENTRY(set3DCustomRolloff)
    METHODS_TABLE_ENTRY(get3DCustomRolloff)
#endif
    METHODS_TABLE_ENTRY(getSubSound)
    METHODS_TABLE_ENTRY(getSubSoundParent)
#if 0
    METHODS_TABLE_ENTRY(getName)
    METHODS_TABLE_ENTRY(getLength)
    METHODS_TABLE_ENTRY(getFormat)
#endif
    METHODS_TABLE_ENTRY(getNumSubSounds)
#if 0
    METHODS_TABLE_ENTRY(getNumTags)
    METHODS_TABLE_ENTRY(getTag)
    METHODS_TABLE_ENTRY(getOpenState)
    METHODS_TABLE_ENTRY(readData)
    METHODS_TABLE_ENTRY(seekData)
    METHODS_TABLE_ENTRY(setSoundGroup)
    METHODS_TABLE_ENTRY(getSoundGroup)
    METHODS_TABLE_ENTRY(getNumSyncPoints)
    METHODS_TABLE_ENTRY(getSyncPoint)
    METHODS_TABLE_ENTRY(getSyncPointInfo)
    METHODS_TABLE_ENTRY(addSyncPoint)
    METHODS_TABLE_ENTRY(deleteSyncPoint)
#endif
    METHODS_TABLE_ENTRY(setMode)
    METHODS_TABLE_ENTRY(getMode)
    METHODS_TABLE_ENTRY(setLoopCount)
    METHODS_TABLE_ENTRY(getLoopCount)
#if 0
    METHODS_TABLE_ENTRY(setLoopPoints)
    METHODS_TABLE_ENTRY(getLoopPoints)
#endif
    METHODS_TABLE_ENTRY(getMusicNumChannels)
    METHODS_TABLE_ENTRY(setMusicChannelVolume)
    METHODS_TABLE_ENTRY(getMusicChannelVolume)
    METHODS_TABLE_ENTRY(setMusicSpeed)
    METHODS_TABLE_ENTRY(getMusicSpeed)
#if 0
    METHODS_TABLE_ENTRY(setUserData)
    METHODS_TABLE_ENTRY(getUserData)
#endif
METHODS_TABLE_END
