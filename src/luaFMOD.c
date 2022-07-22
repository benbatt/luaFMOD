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

#include <fmod_errors.h>
#include <fmod_studio.h>
#include <lauxlib.h>

#define BANK_METATABLE "FMOD.Studio.Bank"
#define EVENTDESCRIPTION_METATABLE "FMOD.Studio.EventDescription"
#define EVENTINSTANCE_METATABLE "FMOD.Studio.EventInstance"
#define SYSTEM_METATABLE "FMOD.Studio.System"

/* If an FMOD error is encountered, return nil and the error code */
#define REQUIRE_OK(result) \
    do { \
        FMOD_RESULT _result = (result); \
        if (_result != FMOD_OK) { \
            return luaL_error(L, "FMOD error %d: %s", _result, FMOD_ErrorString(_result)); \
        } \
    } while(0)

#define RETURN_IF_ERROR(result) \
    do { \
        FMOD_RESULT _result = (result); \
        if (_result != FMOD_OK) { \
            lua_pushnil(L); \
            lua_pushinteger(L, _result); \
            return 2; \
        } \
    } while(0)

#define GET_SELF(type) \
    FMOD_STUDIO_ ## type *self = *((FMOD_STUDIO_ ## type **)luaL_checkudata(L, 1, type ## _METATABLE));

#define CREATE_USERDATA(type, value) \
    do { \
        *((FMOD_STUDIO_ ## type **)lua_newuserdata(L, sizeof(value))) = (value); \
        luaL_getmetatable(L, type ## _METATABLE); \
        lua_setmetatable(L, -2); \
    } while(0)

static int Studio_System_Create(lua_State *L)
{
    FMOD_STUDIO_SYSTEM *system = NULL;

    REQUIRE_OK(FMOD_Studio_System_Create(&system, FMOD_VERSION));

    CREATE_USERDATA(SYSTEM, system);

    return 1;
}

static int Studio_System_Initialize(lua_State *L)
{
    GET_SELF(SYSTEM);

    int maxchannels = luaL_checkint(L, 2);
    int studioflags = luaL_checkint(L, 3);
    int coreflags = luaL_checkint(L, 4);

    /* TODO extradriverdata? */

    REQUIRE_OK(FMOD_Studio_System_Initialize(self, maxchannels, studioflags, coreflags, NULL));

    return 0;
}

static int Studio_System_Release(lua_State *L)
{
    GET_SELF(SYSTEM);

    REQUIRE_OK(FMOD_Studio_System_Release(self));

    return 0;
}

static int Studio_System_Update(lua_State *L)
{
    GET_SELF(SYSTEM);

    REQUIRE_OK(FMOD_Studio_System_Update(self));

    return 0;
}

static int Studio_System_LoadBankFile(lua_State *L)
{
    GET_SELF(SYSTEM);

    const char *filename = luaL_checkstring(L, 2);
    int flags = luaL_checkint(L, 3);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_LoadBankFile(self, filename, flags, &bank));

    CREATE_USERDATA(BANK, bank);

    return 1;
}

static int Studio_System_GetEvent(lua_State *L)
{
    GET_SELF(SYSTEM);

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetEvent(self, path, &description));

    CREATE_USERDATA(EVENTDESCRIPTION, description);

    return 1;
}

static const struct luaL_reg System_Static[] = {
    { "create", Studio_System_Create },
    { NULL, NULL }
};

static const struct luaL_reg SystemMethods[] = {
    { "initialize", Studio_System_Initialize },
    { "release", Studio_System_Release },
    { "update", Studio_System_Update },
    { "loadBankFile", Studio_System_LoadBankFile },
    { "getEvent", Studio_System_GetEvent },
    { NULL, NULL }
};

static const struct luaL_reg BankMethods[] = {
    { NULL, NULL }
};

static int Studio_EventDescription_CreateInstance(lua_State *L)
{
    GET_SELF(EVENTDESCRIPTION);

    FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
    RETURN_IF_ERROR(FMOD_Studio_EventDescription_CreateInstance(self, &instance));

    CREATE_USERDATA(EVENTINSTANCE, instance);

    return 1;
}

static const struct luaL_reg EventDescriptionMethods[] = {
    { "createInstance", Studio_EventDescription_CreateInstance },
    { NULL, NULL }
};

static int Studio_EventInstance_Start(lua_State *L)
{
    GET_SELF(EVENTINSTANCE);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_Start(self));

    lua_pushboolean(L, 1);

    return 1;
}

static int Studio_EventInstance_SetParameterByName(lua_State *L)
{
    GET_SELF(EVENTINSTANCE);

    const char *name = luaL_checkstring(L, 2);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_IF_ERROR(FMOD_Studio_EventInstance_SetParameterByName(self, name, value, ignoreseekspeed));

    lua_pushboolean(L, 1);

    return 1;
}

static const struct luaL_reg EventInstanceMethods[] = {
    { "start", Studio_EventInstance_Start },
    { "setParameterByName", Studio_EventInstance_SetParameterByName },
    { NULL, NULL }
};

static const struct luaL_reg FMOD[] = {
    { NULL, NULL }
};

static void createMethodsTable(lua_State *L, const char *name, const luaL_reg *methods)
{
    luaL_newmetatable(L, name);

    /* Set the metatable as its own __index, so userdata with this metatable
       will look inside it for method definitions.
    */
    lua_pushvalue(L, -1);           /* copy the metatable to the stack top */
    lua_setfield(L, -2, "__index"); /* set the __index field */

    luaL_register(L, NULL, methods);
}

extern int luaopen_FMOD(lua_State *L)
{
    /* The FMOD table */
    luaL_register(L, "FMOD", FMOD);

    /* The FMOD.Studio table */
    lua_createtable(L, 0, 1);

    /* The FMOD.Studio.System table */
    lua_createtable(L, 0, 1);
    luaL_register(L, NULL, System_Static);

    /* Set FMOD.Studio.System */
    lua_setfield(L, -2, "System");

    /* Set FMOD.Studio */
    lua_setfield(L, -2, "Studio");

    /* Create the class method tables */
    createMethodsTable(L, BANK_METATABLE, BankMethods);
    createMethodsTable(L, EVENTDESCRIPTION_METATABLE, EventDescriptionMethods);
    createMethodsTable(L, EVENTINSTANCE_METATABLE, EventInstanceMethods);
    createMethodsTable(L, SYSTEM_METATABLE, SystemMethods);

    return 1;
}
