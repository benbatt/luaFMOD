#ifndef COMMON_H
#define COMMON_H

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

#define CONSTANT_ACCESS_DECLARE(type) \
    int CONSTANT_access_ ## type(lua_State *L, type *data, int set, int valueIndex)

#define STRUCT_REQUIRED 1
#define STRUCT_OPTIONAL 0

void *STRUCT_todata(lua_State *L, const char *metatable, int index, int required);

#define CHECK_STRUCT(L, index, type) ((type*)STRUCT_todata(L, # type, index, STRUCT_REQUIRED))
#define OPTIONAL_STRUCT(L, index, type) ((type*)STRUCT_todata(L, # type, index, STRUCT_OPTIONAL))

#define STRUCT_NEW_DECLARE(type) \
    int type ## _new(lua_State *L)

#define STRUCT_NEWREF_DECLARE(type) \
    int type ## _newref(lua_State *L, int parentIndex, type *data)

STRUCT_NEW_DECLARE(FMOD_STUDIO_PARAMETER_DESCRIPTION);
STRUCT_NEWREF_DECLARE(FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES);

#define STRINGIZE(x) STRINGIZE_IMPL(x)

#define STRINGIZE_IMPL(x) #x

#define JOIN(a, b) JOIN_IMPL(a, b)
#define JOIN4(a, b, c, d) JOIN(JOIN(a, b), JOIN(c, d))

#define JOIN_IMPL(a, b) a ## b

int getOptionalConstant(lua_State *L, int index, const char *metatable, int defaultValue);

#define CHECK_CONSTANT(index, type) *(int*)luaL_checkudata(L, index, # type)
#define OPTIONAL_CONSTANT(index, type, defaultValue) getOptionalConstant(L, index, # type, defaultValue)

int CONSTANT_new(lua_State *L, const char *metatable, int value);

#define PUSH_CONSTANT(L, type, value) CONSTANT_new(L, #type, value)

/* If an FMOD error is encountered, raise a Lua error */
#define REQUIRE_OK(result) \
    do { \
        FMOD_RESULT _result = (result); \
        if (_result != FMOD_OK) { \
            return luaL_error(L, "FMOD error %d: %s", _result, FMOD_ErrorString(_result)); \
        } \
    } while(0)

/* If an FMOD error is encountered, return nil and the error code */
#define RETURN_IF_ERROR(result) \
    do { \
        FMOD_RESULT _result = (result); \
        if (_result != FMOD_OK) { \
            lua_pushnil(L); \
            lua_pushfstring(L, "FMOD error %d: %s", _result, FMOD_ErrorString(_result)); \
            lua_pushinteger(L, _result); \
            return 3; \
        } \
    } while(0)

#define RETURN_STATUS(result) \
    do { \
        RETURN_IF_ERROR(result); \
        lua_pushboolean(L, 1); \
        return 1; \
    } while(0)

#define CHECK_HANDLE(L, index, type) *((type **)luaL_checkudata(L, index, STRINGIZE(type)))

#define GET_SELF \
    SELF_TYPE *self = CHECK_HANDLE(L, 1, SELF_TYPE)

#define PUSH_USERDATA(L, type, value) \
    do { \
        *((type **)lua_newuserdata(L, sizeof(value))) = (value); \
        luaL_getmetatable(L, #type); \
        lua_setmetatable(L, -2); \
    } while(0)

#define FUNCTION_TABLE_BEGIN(name) const struct luaL_reg name[] = {
#define FUNCTION_TABLE_ENTRY(function) { #function, function },
#define FUNCTION_TABLE_END { NULL, NULL } };

#define METHODS_TABLE_BEGIN FUNCTION_TABLE_BEGIN(JOIN(SELF_TYPE, _methods))
#define METHODS_TABLE_ENTRY(method) FUNCTION_TABLE_ENTRY(method)
#define METHODS_TABLE_END FUNCTION_TABLE_END

#endif /* COMMON_H */
