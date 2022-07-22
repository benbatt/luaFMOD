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

static int create(lua_State *L)
{
    FMOD_STUDIO_SYSTEM *system = NULL;

    REQUIRE_OK(FMOD_Studio_System_Create(&system, FMOD_VERSION));

    CREATE_USERDATA(SYSTEM, system);

    return 1;
}

static int initialize(lua_State *L)
{
    GET_SELF(SYSTEM);

    int maxchannels = luaL_checkint(L, 2);
    int studioflags = luaL_checkint(L, 3);
    int coreflags = luaL_checkint(L, 4);

    /* TODO extradriverdata? */

    REQUIRE_OK(FMOD_Studio_System_Initialize(self, maxchannels, studioflags, coreflags, NULL));

    return 0;
}

static int release(lua_State *L)
{
    GET_SELF(SYSTEM);

    REQUIRE_OK(FMOD_Studio_System_Release(self));

    return 0;
}

static int update(lua_State *L)
{
    GET_SELF(SYSTEM);

    REQUIRE_OK(FMOD_Studio_System_Update(self));

    return 0;
}

static int loadBankFile(lua_State *L)
{
    GET_SELF(SYSTEM);

    const char *filename = luaL_checkstring(L, 2);
    int flags = luaL_checkint(L, 3);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_LoadBankFile(self, filename, flags, &bank));

    CREATE_USERDATA(BANK, bank);

    return 1;
}

static int getEvent(lua_State *L)
{
    GET_SELF(SYSTEM);

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetEvent(self, path, &description));

    CREATE_USERDATA(EVENTDESCRIPTION, description);

    return 1;
}

FUNCTION_TABLE_BEGIN(SystemStaticFunctions)
    FUNCTION_TABLE_ENTRY(create)
FUNCTION_TABLE_END

FUNCTION_TABLE_BEGIN(SystemMethods)
    FUNCTION_TABLE_ENTRY(initialize)
    FUNCTION_TABLE_ENTRY(release)
    FUNCTION_TABLE_ENTRY(update)
    FUNCTION_TABLE_ENTRY(loadBankFile)
    FUNCTION_TABLE_ENTRY(getEvent)
FUNCTION_TABLE_END
