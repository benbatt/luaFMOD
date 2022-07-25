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

FUNCTION_TABLE_BEGIN(FMODStaticFunctions)
FUNCTION_TABLE_END

static void createMethodsTable(lua_State *L, const char *name, const luaL_reg *methods)
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

USE_FUNCTION_TABLE(StudioSystemStaticFunctions);
USE_FUNCTION_TABLE(StudioSystemMethods);
USE_FUNCTION_TABLE(BankMethods);
USE_FUNCTION_TABLE(EventDescriptionMethods);
USE_FUNCTION_TABLE(EventInstanceMethods);
USE_FUNCTION_TABLE(CoreSystemMethods);

extern void createConstantTables(lua_State *L);

extern int luaopen_FMOD(lua_State *L)
{
    /* The FMOD table */
    luaL_register(L, "FMOD", FMODStaticFunctions);

    /* The FMOD.Studio table */
    lua_createtable(L, 0, 1);

    /* The FMOD.Studio.System table */
    lua_createtable(L, 0, 1);
    luaL_register(L, NULL, StudioSystemStaticFunctions);

    /* Set FMOD.Studio.System */
    lua_setfield(L, -2, "System");

    /* Set FMOD.Studio */
    lua_setfield(L, -2, "Studio");

    /* Create the class method tables */
    createMethodsTable(L, FMOD_STUDIO_BANK_METATABLE, BankMethods);
    createMethodsTable(L, FMOD_STUDIO_EVENTDESCRIPTION_METATABLE, EventDescriptionMethods);
    createMethodsTable(L, FMOD_STUDIO_EVENTINSTANCE_METATABLE, EventInstanceMethods);
    createMethodsTable(L, FMOD_STUDIO_SYSTEM_METATABLE, StudioSystemMethods);
    createMethodsTable(L, FMOD_SYSTEM_METATABLE, CoreSystemMethods);

    /* Create constants */
    createConstantTables(L);

    return 1;
}
