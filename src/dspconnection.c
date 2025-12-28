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

#define SELF_TYPE FMOD_DSPCONNECTION
#define FMOD_PREFIX FMOD_DSPConnection_

#include "templates.h"

GET(Input, (FMOD_DSP, HANDLE))
GET(Output, (FMOD_DSP, HANDLE))
PROPERTY(Mix, float)

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(getInput)
    METHODS_TABLE_ENTRY(getOutput)
    METHODS_TABLE_ENTRY(setMix)
    METHODS_TABLE_ENTRY(getMix)
#if 0
    METHODS_TABLE_ENTRY(setMixMatrix)
    METHODS_TABLE_ENTRY(getMixMatrix)
    METHODS_TABLE_ENTRY(getType)
    METHODS_TABLE_ENTRY(setUserData)
    METHODS_TABLE_ENTRY(getUserData)
#endif
METHODS_TABLE_END
