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
#include <string.h>

static int FMOD_VECTOR_new(lua_State *L)
{
    FMOD_VECTOR *data = (FMOD_VECTOR*)lua_newuserdata(L, sizeof(*data));

    memset(data, 0, sizeof(*data));

    luaL_getmetatable(L, "FMOD_VECTOR");
    lua_setmetatable(L, -2);

    return 1;
}

static int FMOD_VECTOR_index(lua_State *L)
{
    FMOD_VECTOR *data = luaL_checkudata(L, 1, "FMOD_VECTOR");

    luaL_argcheck(L, lua_type(L, 2) == LUA_TSTRING, 2, "Field name must be a string");

    size_t length = 0;
    const char *field = lua_tolstring(L, 2, &length);

    if (strncmp(field, "x", length) == 0)
    {
        lua_pushnumber(L, data->x);
        return 1;
    }
    if (strncmp(field, "y", length) == 0)
    {
        lua_pushnumber(L, data->y);
        return 1;
    }
    if (strncmp(field, "z", length) == 0)
    {
        lua_pushnumber(L, data->z);
        return 1;
    }

    return luaL_error(L, "Invalid field %s.%s", "FMOD_VECTOR", field);
}

static int FMOD_VECTOR_newindex(lua_State *L)
{
    FMOD_VECTOR *data = luaL_checkudata(L, 1, "FMOD_VECTOR");

    luaL_argcheck(L, lua_type(L, 2) == LUA_TSTRING, 2, "Field name must be a string");

    size_t length = 0;
    const char *field = lua_tolstring(L, 2, &length);

    if (strncmp(field, "x", length) == 0)
    {
        data->x = luaL_checknumber(L, 3);
        return 0;
    }
    if (strncmp(field, "y", length) == 0)
    {
        data->y = luaL_checknumber(L, 3);
        return 0;
    }
    if (strncmp(field, "z", length) == 0)
    {
        data->z = luaL_checknumber(L, 3);
        return 0;
    }

    return luaL_error(L, "Invalid field %s.%s", "FMOD_VECTOR", field);
}

static void createVectorStruct(lua_State *L)
{
    lua_createtable(L, 0, 0);

    lua_pushcfunction(L, FMOD_VECTOR_new);
    lua_setfield(L, -2, "new");

    lua_setfield(L, -2, "VECTOR");

    /* Create the metatable for FMOD_VECTOR objects */
    luaL_newmetatable(L, "FMOD_VECTOR");

    lua_pushcfunction(L, FMOD_VECTOR_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, FMOD_VECTOR_newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
}

void createStructTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack, so define FMOD structs first */
    createVectorStruct(L);

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
