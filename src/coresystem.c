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

static int setSoftwareFormat(lua_State *L)
{
    GET_SELF(FMOD_SYSTEM);

    int samplerate = luaL_checkint(L, 2);
    int speakermode = CHECK_CONSTANT(3, FMOD_SPEAKERMODE);
    int numrawspeakers = luaL_checkint(L, 4);

    REQUIRE_OK(FMOD_System_SetSoftwareFormat(self, samplerate, speakermode, numrawspeakers));

    return 0;
}

FUNCTION_TABLE_BEGIN(CoreSystemMethods)
    FUNCTION_TABLE_ENTRY(setSoftwareFormat)
FUNCTION_TABLE_END
