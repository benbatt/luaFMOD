/*
Copyright 2025 Ben Batt

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

#define SELF_TYPE FMOD_DSP
#define FMOD_PREFIX FMOD_DSP_

#include "templates.h"

GET(SystemObject, (FMOD_SYSTEM, HANDLE))
GET(NumInputs, int)
GET(NumOutputs, int)
GET_INDEXED_MULTI(Input, (FMOD_DSP, HANDLE), (FMOD_DSPCONNECTION, HANDLE))
GET_INDEXED_MULTI(Output, (FMOD_DSP, HANDLE), (FMOD_DSPCONNECTION, HANDLE))
PROPERTY(Active, FMOD_BOOL)
PROPERTY(Bypass, FMOD_BOOL)
PROPERTY_MULTI(WetDryMix, float, float, float)
GET(NumParameters, int)
GET(Idle, FMOD_BOOL)
PROPERTY_MULTI(MeteringEnabled, FMOD_BOOL, FMOD_BOOL)
GET_MULTI(CPUUsage, unsigned, unsigned)

METHODS_TABLE_BEGIN
#if 0
    METHODS_TABLE_ENTRY(release)
#endif
    METHODS_TABLE_ENTRY(getSystemObject)
#if 0
    METHODS_TABLE_ENTRY(addInput)
    METHODS_TABLE_ENTRY(disconnectFrom)
    METHODS_TABLE_ENTRY(disconnectAll)
#endif
    METHODS_TABLE_ENTRY(getNumInputs)
    METHODS_TABLE_ENTRY(getNumOutputs)
    METHODS_TABLE_ENTRY(getInput)
    METHODS_TABLE_ENTRY(getOutput)
    METHODS_TABLE_ENTRY(setActive)
    METHODS_TABLE_ENTRY(getActive)
    METHODS_TABLE_ENTRY(setBypass)
    METHODS_TABLE_ENTRY(getBypass)
    METHODS_TABLE_ENTRY(setWetDryMix)
    METHODS_TABLE_ENTRY(getWetDryMix)
#if 0
    METHODS_TABLE_ENTRY(setChannelFormat)
    METHODS_TABLE_ENTRY(getChannelFormat)
    METHODS_TABLE_ENTRY(getOutputChannelFormat)
    METHODS_TABLE_ENTRY(reset)
    METHODS_TABLE_ENTRY(setParameterFloat)
    METHODS_TABLE_ENTRY(setParameterInt)
    METHODS_TABLE_ENTRY(setParameterBool)
    METHODS_TABLE_ENTRY(setParameterData)
    METHODS_TABLE_ENTRY(getParameterFloat)
    METHODS_TABLE_ENTRY(getParameterInt)
    METHODS_TABLE_ENTRY(getParameterBool)
    METHODS_TABLE_ENTRY(getParameterData)
#endif
    METHODS_TABLE_ENTRY(getNumParameters)
#if 0
    METHODS_TABLE_ENTRY(getParameterInfo)
    METHODS_TABLE_ENTRY(getDataParameterIndex)
    METHODS_TABLE_ENTRY(showConfigDialog)
    METHODS_TABLE_ENTRY(getInfo)
    METHODS_TABLE_ENTRY(getType)
#endif
    METHODS_TABLE_ENTRY(getIdle)
#if 0
    METHODS_TABLE_ENTRY(setUserData)
    METHODS_TABLE_ENTRY(getUserData)
#endif
    METHODS_TABLE_ENTRY(setMeteringEnabled)
    METHODS_TABLE_ENTRY(getMeteringEnabled)
#if 0
    METHODS_TABLE_ENTRY(getMeteringInfo)
#endif
    METHODS_TABLE_ENTRY(getCPUUsage)
METHODS_TABLE_END
