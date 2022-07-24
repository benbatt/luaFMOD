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

#define BANK_METATABLE "FMOD.Studio.Bank"
#define EVENTDESCRIPTION_METATABLE "FMOD.Studio.EventDescription"
#define EVENTINSTANCE_METATABLE "FMOD.Studio.EventInstance"
#define SYSTEM_METATABLE "FMOD.Studio.System"

#define STRINGIZE(x) STRINGIZE_IMPL(x)

#define STRINGIZE_IMPL(x) #x

#define CHECK_CONSTANT(index, metatableName) *(int*)luaL_checkudata(L, index, #metatableName);

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

#define FUNCTION_TABLE_BEGIN(name) const struct luaL_reg name[] = {
#define FUNCTION_TABLE_ENTRY(function) { #function, function },
#define FUNCTION_TABLE_END { NULL, NULL } };

#define USE_FUNCTION_TABLE(name) extern const struct luaL_reg name[];

#endif /* COMMON_H */
