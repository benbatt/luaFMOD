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
#include "logging.h"

#define SELF_TYPE FMOD_STUDIO_SYSTEM

static int create(lua_State *L)
{
    FMOD_STUDIO_SYSTEM *system = NULL;

    REQUIRE_OK(FMOD_Studio_System_Create(&system, FMOD_VERSION));

    PUSH_HANDLE(L, FMOD_STUDIO_SYSTEM, system);

    return 1;
}

static int setAdvancedSettings(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_ADVANCEDSETTINGS *settings = CHECK_STRUCT(L, 2, FMOD_STUDIO_ADVANCEDSETTINGS);
    settings->cbsize = sizeof(*settings);

    RETURN_STATUS(FMOD_Studio_System_SetAdvancedSettings(self, settings));
}

static int getAdvancedSettings(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_ADVANCEDSETTINGS settings;
    settings.cbsize = sizeof(settings);

    RETURN_IF_ERROR(FMOD_Studio_System_GetAdvancedSettings(self, &settings));

    PUSH_STRUCT(L, FMOD_STUDIO_ADVANCEDSETTINGS, settings);

    return 1;
}

static int initialize(lua_State *L)
{
    GET_SELF;

    int maxchannels = luaL_checkint(L, 2);
    int studioflags = CHECK_CONSTANT(L, 3, FMOD_STUDIO_INITFLAGS);
    int coreflags = CHECK_CONSTANT(L, 4, FMOD_INITFLAGS);

    /* TODO extradriverdata? */

    REQUIRE_OK(FMOD_Studio_System_Initialize(self, maxchannels, studioflags, coreflags, NULL));

    return 0;
}

static int release(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_Studio_System_Release(self));

    return 0;
}

static int update(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_Studio_System_Update(self));

    loggingPumpMessages(L);

    return 0;
}

static int getCoreSystem(lua_State *L)
{
    GET_SELF;

    FMOD_SYSTEM *system = NULL;
    REQUIRE_OK(FMOD_Studio_System_GetCoreSystem(self, &system));

    PUSH_HANDLE(L, FMOD_SYSTEM, system);

    return 1;
}

static int getEvent(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetEvent(self, path, &description));

    PUSH_HANDLE(L, FMOD_STUDIO_EVENTDESCRIPTION, description);

    return 1;
}

static int getBus(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_BUS *bus = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBus(self, path, &bus));

    PUSH_HANDLE(L, FMOD_STUDIO_BUS, bus);

    return 1;
}

static int getVCA(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_VCA *vca = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetVCA(self, path, &vca));

    PUSH_HANDLE(L, FMOD_STUDIO_VCA, vca);

    return 1;
}

static int getBank(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBank(self, path, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

static int setListenerAttributes(lua_State *L)
{
    GET_SELF;

    int listener = luaL_checkint(L, 2);

    FMOD_3D_ATTRIBUTES *attributes = CHECK_STRUCT(L, 3, FMOD_3D_ATTRIBUTES);
    FMOD_VECTOR *attenuationposition = OPTIONAL_STRUCT(L, 4, FMOD_VECTOR);

    RETURN_STATUS(FMOD_Studio_System_SetListenerAttributes(self, listener, attributes, attenuationposition));
}

static int loadBankFile(lua_State *L)
{
    GET_SELF;

    const char *filename = luaL_checkstring(L, 2);
    int flags = CHECK_CONSTANT(L, 3, FMOD_STUDIO_LOAD_BANK_FLAGS);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_LoadBankFile(self, filename, flags, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

FUNCTION_TABLE_BEGIN(StudioSystemStaticFunctions)
    FUNCTION_TABLE_ENTRY(create)
FUNCTION_TABLE_END

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(setAdvancedSettings)
    METHODS_TABLE_ENTRY(getAdvancedSettings)
    METHODS_TABLE_ENTRY(initialize)
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(update)
    METHODS_TABLE_ENTRY(getCoreSystem)
    METHODS_TABLE_ENTRY(getEvent)
    METHODS_TABLE_ENTRY(getBus)
    METHODS_TABLE_ENTRY(getVCA)
    METHODS_TABLE_ENTRY(getBank)
    METHODS_TABLE_ENTRY(setListenerAttributes)
    METHODS_TABLE_ENTRY(loadBankFile)
METHODS_TABLE_END
