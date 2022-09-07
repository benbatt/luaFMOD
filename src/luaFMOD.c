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
#include "platform.h"
#include "logging.h"

static int Debug_Initialize(lua_State *L)
{
    int flags = CHECK_CONSTANT(L, 1, FMOD_DEBUG_FLAGS);
    int mode = OPTIONAL_CONSTANT(L, 2, FMOD_DEBUG_MODE, FMOD_DEBUG_MODE_TTY);

    int callback = 0;

    if (!lua_isnil(L, 3)) {
        lua_pushvalue(L, 3);
        callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    const char *filename = lua_tostring(L, 4);

    REQUIRE_OK(loggingStart(flags, mode, callback, filename));

    return 0;
}

FUNCTION_TABLE_BEGIN(CoreStaticFunctions)
    FUNCTION_TABLE_ENTRY(Debug_Initialize)
FUNCTION_TABLE_END

static int parseID(lua_State *L)
{
    const char *idstring = luaL_checkstring(L, 1);

    FMOD_GUID id;
    RETURN_IF_ERROR(FMOD_Studio_ParseID(idstring, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

FUNCTION_TABLE_BEGIN(StudioStaticFunctions)
    FUNCTION_TABLE_ENTRY(parseID)
FUNCTION_TABLE_END

#define REGISTER_FUNCTION_TABLE(L, name, table) \
    do { \
        extern const struct luaL_reg table[]; \
        luaL_register(L, name, table); \
    } while (0)

static void registerMethodsTable(lua_State *L, const char *name, const luaL_reg *methods)
{
    luaL_newmetatable(L, name);

    /* Set the metatable as its own __index, so userdata with this metatable
       will look inside it for method definitions.
    */
    lua_pushvalue(L, -1);           /* copy the metatable to the stack top */
    lua_setfield(L, -2, "__index"); /* set the __index field */

    luaL_register(L, NULL, methods);

    lua_pop(L, 1);
}

#define REGISTER_METHODS_TABLE(L, type) \
    do { \
        extern const struct luaL_reg type ## _methods[]; \
        registerMethodsTable(L, #type, type ## _methods); \
    } while (0)

extern void createConstantTables(lua_State *L);
extern void createStructTables(lua_State *L);

extern int LUAFMOD_EXPORT luaopen_luaFMOD(lua_State *L)
{
    platformInitialize(L);

    /* The FMOD table */
    REGISTER_FUNCTION_TABLE(L, "FMOD", CoreStaticFunctions);

    /* The FMOD.Studio table */
    lua_createtable(L, 0, 1);
    REGISTER_FUNCTION_TABLE(L, NULL, StudioStaticFunctions);

    /* The FMOD.Studio.System table */
    lua_createtable(L, 0, 1);
    REGISTER_FUNCTION_TABLE(L, NULL, StudioSystemStaticFunctions);

    /* Set FMOD.Studio.System */
    lua_setfield(L, -2, "System");

    /* Set FMOD.Studio */
    lua_setfield(L, -2, "Studio");

    /* Create the class method tables */
    REGISTER_METHODS_TABLE(L, FMOD_STUDIO_BANK);
    REGISTER_METHODS_TABLE(L, FMOD_STUDIO_EVENTDESCRIPTION);
    REGISTER_METHODS_TABLE(L, FMOD_STUDIO_EVENTINSTANCE);
    REGISTER_METHODS_TABLE(L, FMOD_STUDIO_SYSTEM);
    REGISTER_METHODS_TABLE(L, FMOD_SYSTEM);
    REGISTER_METHODS_TABLE(L, FMOD_SOUND);
    REGISTER_METHODS_TABLE(L, FMOD_CHANNEL);

    /* Create constants */
    createConstantTables(L);

    /* Create structs */
    createStructTables(L);

    return 1;
}
