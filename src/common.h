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

#define STRINGIZE(x) STRINGIZE_IMPL(x)
#define STRINGIZE_IMPL(x) #x

#define JOIN(a, b) JOIN_IMPL(a, b)
#define JOIN_IMPL(a, b) a ## b

#define CHECK_HANDLE(L, index, type) *((type **)luaL_checkudata(L, index, STRINGIZE(type)))

#define GET_SELF \
    SELF_TYPE *self = CHECK_HANDLE(L, 1, SELF_TYPE)

#define PUSH_HANDLE(L, type, value) \
    do { \
        *((type **)lua_newuserdata(L, sizeof(value))) = (value); \
        luaL_getmetatable(L, #type); \
        lua_setmetatable(L, -2); \
    } while(0)

#define IS_STRUCT(L, index, type) STRUCT_is(L, # type, index)
#define CHECK_STRUCT(L, index, type) ((type*)STRUCT_todata(L, # type, index, STRUCT_REQUIRED))
#define OPTIONAL_STRUCT(L, index, type) ((type*)STRUCT_todata(L, # type, index, STRUCT_OPTIONAL))

#define PUSH_STRUCT(L, type, value) \
    do { \
        STRUCT_new(L, # type, sizeof(type)); \
        *CHECK_STRUCT(L, -1, type) = (value); \
    } while(0)

/* parentIndex is the stack index of the containing object, or 0 for no containing object */
#define PUSH_STRUCT_REF(L, type, parentIndex, pointer) STRUCT_newref(L, # type, parentIndex, pointer)

#define CHECK_CONSTANT(L, index, type) *(int*)luaL_checkudata(L, index, # type)
#define OPTIONAL_CONSTANT(L, index, type, defaultValue) getOptionalConstant(L, index, # type, defaultValue)

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
#define RETURN_IF_ERROR(result, ...) \
    do { \
        FMOD_RESULT _result = (result); \
        if (_result != FMOD_OK) { \
            __VA_ARGS__ \
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

typedef struct {
    size_t size;
    char fixedBuffer[256];
} StackBufferInfo;

#define STACKBUFFER_CREATE(type, name, count) \
    StackBufferInfo _ ## name ## _info = { sizeof(type) * count }; \
    type *name = NULL; \
    do { \
        name = stackBufferSelect(L, &_ ## name ## _info); \
    } while(0)

#define STACKBUFFER_RELEASE(name) \
    do { \
        stackBufferRelease(L, name, &_ ## name ## _info); \
    } while(0)

#define FUNCTION_TABLE_BEGIN(name) const struct luaL_reg name[] = {
#define FUNCTION_TABLE_ENTRY(function) { #function, function },
#define FUNCTION_TABLE_END { NULL, NULL } };

#define METHODS_TABLE_BEGIN FUNCTION_TABLE_BEGIN(JOIN(SELF_TYPE, _methods))
#define METHODS_TABLE_ENTRY(method) FUNCTION_TABLE_ENTRY(method)
#define METHODS_TABLE_END FUNCTION_TABLE_END

#define CONSTANT_ACCESS_DECLARE(type) \
    int CONSTANT_access_ ## type(lua_State *L, type *data, int set, int valueIndex)

#define STRUCT_REQUIRED 1
#define STRUCT_OPTIONAL 0

void *stackBufferSelect(lua_State *L, StackBufferInfo *info);
void stackBufferRelease(lua_State *L, void *pointer, StackBufferInfo *info);

typedef const char * luaFMOD_Buffer;

int STRUCT_new(lua_State *L, const char *metatable, size_t size);
int STRUCT_newref(lua_State *L, const char *metatable, int parentIndex, const void *data);
int STRUCT_is(lua_State *L, const char *metatable, int index);
void *STRUCT_todata(lua_State *L, const char *metatable, int index, int required);

int getOptionalConstant(lua_State *L, int index, const char *metatable, int defaultValue);
int CONSTANT_new(lua_State *L, const char *metatable, int value);

void getParameterIDsAndValues(lua_State *L, const int IDS_INDEX, const int VALUES_INDEX,
    FMOD_STUDIO_PARAMETER_ID **idsOut, float **valuesOut, int *countOut);

#endif /* COMMON_H */
