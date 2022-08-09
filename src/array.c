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

typedef unsigned char (uchar8)[8];

static int ARRAY_uchar8_newref(lua_State *L, int parentIndex, uchar8 *data)
{
    return STRUCT_newref(L, "ARRAY_uchar8", parentIndex, data);
}

static int ARRAY_uchar8_gc(lua_State *L)
{
    return STRUCT_gc(L, "ARRAY_uchar8");
}

static uchar8 *ARRAY_uchar8_todata(lua_State *L, int index, int required)
{
    return (uchar8*)STRUCT_todata(L, "ARRAY_uchar8", index, required);
}

static int ARRAY_uchar8_elementaccess(lua_State *L, int index, int set);

static int ARRAY_uchar8_index(lua_State *L)
{
    return ARRAY_uchar8_elementaccess(L, 1, 0);
}

static int ARRAY_uchar8_newindex(lua_State *L)
{
    return ARRAY_uchar8_elementaccess(L, 1, 1);
}

static void ARRAY_uchar8_create(lua_State *L)
{
    STRUCT_create(L, NULL, "ARRAY_uchar8",
        NULL, ARRAY_uchar8_gc, ARRAY_uchar8_index, ARRAY_uchar8_newindex);
}

static int ARRAY_uchar8_elementaccess(lua_State *L, int index, int set)
{
    uchar8 *data = ARRAY_uchar8_todata(L, index, STRUCT_REQUIRED);

    int elementIndex = luaL_checkint(L, index + 1);

    if (0 <= elementIndex && elementIndex < 8) {
        return STRUCT_access_uchar(L, (*data) + elementIndex, index, set, index + 2);
    }

    return luaL_error(L, "Invalid index %d", elementIndex);
}

static int STRUCT_access_ARRAY_uchar8(lua_State *L, uchar8 *data, int parentIndex, int set, int valueIndex)
{
    if (set) {
        return luaL_error(L, "Attempt to set a read-only field");
    } else {
        return ARRAY_uchar8_newref(L, parentIndex, data);
    }
}
