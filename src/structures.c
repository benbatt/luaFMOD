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

static int STRUCT_new(lua_State *L, const char *metatable, size_t size)
{
    int *reference = (int*)lua_newuserdata(L, sizeof(int) + size);

    *reference = LUA_NOREF;

    void *data = (void*)(reference + 1);

    memset(data, 0, size);

    luaL_getmetatable(L, metatable);
    lua_setmetatable(L, -2);

    return 1;
}

/* Expects the containing object at parentIndex */
static int STRUCT_newref(lua_State *L, const char *metatable, int parentIndex, void *data)
{
    int *reference = (int*)lua_newuserdata(L, sizeof(int) + sizeof(data));

    /* Store a reference to the containing object */
    lua_pushvalue(L, parentIndex);
    *reference = luaL_ref(L, LUA_REGISTRYINDEX);

    *((void**)(reference + 1)) = data;

    luaL_getmetatable(L, metatable);
    lua_setmetatable(L, -2);

    return 1;
}

static int STRUCT_gc(lua_State *L, const char *metatable)
{
    int *reference = (int*)luaL_checkudata(L, 1, metatable);

    if (*reference != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, *reference);
    }

    return 0;
}

static void *STRUCT_data(lua_State *L, const char *metatable, int index)
{
    int *reference = (int*)luaL_checkudata(L, index, metatable);

    if (*reference == LUA_NOREF)
    {
        return reference + 1;
    }
    else
    {
        return *((void**)(reference + 1));
    }
}

static void STRUCT_create(lua_State *L, const char *fieldName, const char *metatable,
    lua_CFunction new, lua_CFunction gc, lua_CFunction index, lua_CFunction newindex)
{
    lua_createtable(L, 0, 0);

    lua_pushcfunction(L, new);
    lua_setfield(L, -2, "new");

    lua_setfield(L, -2, fieldName);

    /* Create the instance metatable  */
    luaL_newmetatable(L, metatable);

    lua_pushcfunction(L, gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
}

static const char *STRUCT_fieldname(lua_State *L, int index, size_t *length)
{
    luaL_argcheck(L, lua_type(L, index) == LUA_TSTRING, index, "Field name must be a string");

    return lua_tolstring(L, index, length);
}

static int STRUCT_access_float(lua_State *L, float *data, int parentIndex, int set, int valueIndex)
{
    if (set)
    {
        *data = luaL_checknumber(L, valueIndex);
        return 0;
    }
    else
    {
        lua_pushnumber(L, *data);
        return 1;
    }
}

#define STRUCT_BEGIN(type) \
    STRUCT_NEW(type) \
    STRUCT_NEWREF(type) \
    STRUCT_GC(type) \
    STRUCT_DATA(type) \
    static int type ## _fieldaccess(lua_State *L, int index, int set); \
    STRUCT_INDEX(type) \
    STRUCT_NEWINDEX(type) \
    STRUCT_ACCESS(type) \
    STRUCT_CREATE(type) \
    STRUCT_FIELDACCESS_BEGIN(type)

#define STRUCT_END STRUCT_FIELDACCESS_END

#define STRUCT_NEW(type) \
    static int type ## _new(lua_State *L) \
    { \
        return STRUCT_new(L, # type, sizeof(type)); \
    }

#define STRUCT_NEWREF(type) \
    static int type ## _newref(lua_State *L, int parentIndex, type *data) \
    { \
        return STRUCT_newref(L, # type, parentIndex, data); \
    }

#define STRUCT_GC(type) \
    static int type ## _gc(lua_State *L) \
    { \
        return STRUCT_gc(L, # type); \
    }

#define STRUCT_DATA(type) \
    static type *type ## _data(lua_State *L, int index) \
    { \
        return (type*)STRUCT_data(L, # type, index); \
    }

#define STRUCT_INDEX(type) \
    static int type ## _index(lua_State *L) \
    { \
        return type ## _fieldaccess(L, 1, 0); \
    }

#define STRUCT_NEWINDEX(type) \
    static int type ## _newindex(lua_State *L) \
    { \
        return type ## _fieldaccess(L, 1, 1); \
    }

#define STRUCT_ACCESS(type) \
    static int STRUCT_access_ ## type(lua_State *L, type *data, int parentIndex, int set, int valueIndex) \
    { \
        if (set) \
        { \
            *data = *type ## _data(L, valueIndex); \
            return 0; \
        } \
        else \
        { \
            return type ## _newref(L, parentIndex, data); \
        } \
    }

#define STRUCT_CREATE(type) \
    static void type ## _create(lua_State *L, const char *fieldName) \
    { \
        STRUCT_create(L, fieldName, # type, type ## _new, type ## _gc, type ## _index, type ## _newindex); \
    }

#define STRUCT_FIELDACCESS_BEGIN(type) \
    static int type ## _fieldaccess(lua_State *L, int index, int set) \
    { \
        static const char *typeName = # type; \
        type *data = type ## _data(L, index); \
        size_t length = 0; \
        const char *field = STRUCT_fieldname(L, index + 1, &length);

#define STRUCT_FIELD(name, type) \
        if (strncmp(# name, field, length) == 0) \
        { \
            return STRUCT_access_ ## type(L, &data->name, index, set, index + 2); \
        }

#define STRUCT_FIELDACCESS_END \
        return luaL_error(L, "Invalid field %s.%s", typeName, field); \
    }

STRUCT_BEGIN(FMOD_VECTOR)
    STRUCT_FIELD(x, float)
    STRUCT_FIELD(y, float)
    STRUCT_FIELD(z, float)
STRUCT_END

STRUCT_BEGIN(FMOD_3D_ATTRIBUTES)
    STRUCT_FIELD(position, FMOD_VECTOR)
    STRUCT_FIELD(velocity, FMOD_VECTOR)
    STRUCT_FIELD(forward, FMOD_VECTOR)
    STRUCT_FIELD(up, FMOD_VECTOR)
STRUCT_END

STRUCT_BEGIN(FMOD_DSP_PARAMETER_3DATTRIBUTES)
    STRUCT_FIELD(relative, FMOD_3D_ATTRIBUTES)
    STRUCT_FIELD(absolute, FMOD_3D_ATTRIBUTES)
STRUCT_END

void createStructTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack, so define FMOD structs first */
    FMOD_VECTOR_create(L, "VECTOR");
    FMOD_3D_ATTRIBUTES_create(L, "_3D_ATTRIBUTES");
    FMOD_DSP_PARAMETER_3DATTRIBUTES_create(L, "DSP_PARAMETER_3DATTRIBUTES");

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);
}
