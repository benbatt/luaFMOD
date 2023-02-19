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

static int sReferenceTable = LUA_NOREF;

/* Pops a struct instance from the top of the stack.
   Pushes the reference table for the struct instance onto the stack.
*/
static void affirmReferenceTable(lua_State *L)
{
    int instanceIndex = lua_gettop(L);

    if (sReferenceTable == LUA_NOREF) {
        /* Create the global reference table */
        lua_newtable(L);

        /* Create the metatable */
        lua_newtable(L);

        /* Use weak keys so instance reference tables get cleaned up automatically */
        lua_pushstring(L, "k");
        lua_setfield(L, -2, "__mode");

        /* Set the metatable on the global reference table */
        lua_setmetatable(L, -2);

        /* Store a reference to the global reference table */
        sReferenceTable = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    /* Get the global reference table */
    lua_rawgeti(L, LUA_REGISTRYINDEX, sReferenceTable);

    int globalIndex = lua_gettop(L);

    /* Get the instance reference table */
    lua_pushvalue(L, instanceIndex);
    lua_gettable(L, globalIndex);

    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);

        /* Create the instance reference table */
        lua_newtable(L);

        /* Add to the global reference table */
        lua_pushvalue(L, instanceIndex);
        lua_pushvalue(L, -2);
        lua_settable(L, globalIndex);
    }

    /* Move the instance reference table into place, and tidy up */
    lua_replace(L, instanceIndex);
    lua_settop(L, instanceIndex);
}

static void STRUCT_setmetatable(lua_State *L, const char *metatable, int index)
{
    luaL_getmetatable(L, metatable);

    if (lua_isnoneornil(L, -1)) {
        luaL_error(L, "The metatable for %s has not been defined", metatable);
    }

    lua_setmetatable(L, index - 1);
}

int STRUCT_new(lua_State *L, const char *metatable, size_t size)
{
    int *reference = (int*)lua_newuserdata(L, sizeof(int) + size);

    *reference = LUA_NOREF;

    void *data = (void*)(reference + 1);

    memset(data, 0, size);

    STRUCT_setmetatable(L, metatable, -1);

    return 1;
}

/* parentIndex is the stack index of the containing object, or 0 for no containing object */
int STRUCT_newref(lua_State *L, const char *metatable, int parentIndex, const void *data)
{
    int *reference = (int*)lua_newuserdata(L, sizeof(int) + sizeof(data));

    if (parentIndex != 0) {
        /* Store a reference to the containing object */
        lua_pushvalue(L, parentIndex);
        *reference = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        *reference = LUA_REFNIL;
    }

    *((const void**)(reference + 1)) = data;

    STRUCT_setmetatable(L, metatable, -1);

    return 1;
}

static int STRUCT_gc(lua_State *L, const char *metatable)
{
    int *reference = (int*)luaL_checkudata(L, 1, metatable);
    luaL_unref(L, LUA_REGISTRYINDEX, *reference);

    return 0;
}

int STRUCT_is(lua_State *L, const char *metatable, int index)
{
    if (lua_type(L, index) != LUA_TUSERDATA) {
        return 0;
    }

    /* We have to get this metatable first so we don't make index refer to the wrong stack element
       in cases where it is negative.
    */
    lua_getmetatable(L, index);
    luaL_getmetatable(L, metatable);

    int metatablesMatch = lua_rawequal(L, -1, -2);

    lua_pop(L, 2);

    return metatablesMatch;
}

void *STRUCT_todata(lua_State *L, const char *metatable, int index, int required)
{
    if (required == STRUCT_OPTIONAL && lua_isnoneornil(L, index)) {
        return NULL;
    }

    int *reference = (int*)luaL_checkudata(L, index, metatable);

    if (*reference == LUA_NOREF) {
        return reference + 1;
    } else {
        return *((void**)(reference + 1));
    }
}

static void STRUCT_create(lua_State *L, const char *fieldName, const char *metatable,
    lua_CFunction new, lua_CFunction gc, lua_CFunction index, lua_CFunction newindex)
{
    if (fieldName) {
        lua_createtable(L, 0, 0);

        lua_pushcfunction(L, new);
        lua_setfield(L, -2, "new");

        lua_setfield(L, -2, fieldName);
    }

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

static int STRUCT_access_float(lua_State *L, const char *fieldName, float *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = (float)luaL_checknumber(L, valueIndex);
        return 0;
    } else {
        lua_pushnumber(L, *data);
        return 1;
    }
}

static int STRUCT_access_int(lua_State *L, const char *fieldName, int *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = luaL_checkint(L, valueIndex);
        return 0;
    } else {
        lua_pushinteger(L, *data);
        return 1;
    }
}

static int STRUCT_access_uint(lua_State *L, const char *fieldName, unsigned int *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = luaL_checkint(L, valueIndex);
        return 0;
    } else {
        lua_pushinteger(L, *data);
        return 1;
    }
}

static int STRUCT_access_ushort(lua_State *L, const char *fieldName, unsigned short *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = luaL_checkint(L, valueIndex);
        return 0;
    } else {
        lua_pushinteger(L, *data);
        return 1;
    }
}

static int STRUCT_access_uchar(lua_State *L, const char *fieldName, unsigned char *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = luaL_checkint(L, valueIndex);
        return 0;
    } else {
        lua_pushinteger(L, *data);
        return 1;
    }
}

static int STRUCT_access_boolean(lua_State *L, const char *fieldName, FMOD_BOOL *data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = lua_toboolean(L, valueIndex);
        return 0;
    } else {
        lua_pushboolean(L, *data);
        return 1;
    }
}

/* Pops struct userdata from the top of the stack.
   Pushes the root struct userdata onto the stack.
*/
static int getRootStruct(lua_State *L)
{
    int *reference = lua_touserdata(L, -1);

    if (!reference) {
        return luaL_error(L, "expected struct userdata at the top of the stack");
    }

    if (*reference == LUA_NOREF || *reference == LUA_REFNIL) {
        return 1;
    } else {
        lua_pop(L, 1);
        lua_rawgeti(L, LUA_REGISTRYINDEX, *reference);
        return getRootStruct(L);
    }
}

static int STRUCT_access_cstring(lua_State *L, const char *fieldName, const char **data, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        lua_pushvalue(L, valueIndex);
        *data = lua_tostring(L, -1);

        int copyIndex = lua_gettop(L);

        /* Keep a reference to the Lua string to prevent garbage collection */
        lua_pushvalue(L, parentIndex);
        getRootStruct(L);
        affirmReferenceTable(L);

        if (*data) {
            lua_pushvalue(L, copyIndex);
        } else {
            lua_pushnil(L);
        }

        lua_setfield(L, -2, fieldName);

        return 0;
    } else {
        lua_pushstring(L, *data);
        return 1;
    }
}

int createHandle(lua_State *L, const char *metatable, void *value)
{
    *((void**)lua_newuserdata(L, sizeof(value))) = value;
    luaL_getmetatable(L, metatable);
    lua_setmetatable(L, -2);

    return 0;
}

static int STRUCT_access_handle(lua_State *L, void **data, const char *metatable, int parentIndex, int set,
    int valueIndex)
{
    if (set) {
        *data = *((void**)luaL_checkudata(L, valueIndex, metatable));
        return 0;
    } else {
        createHandle(L, metatable, *data);
        return 1;
    }
}

#include "array.c"

#define STRUCT_BEGIN(type) \
    STRUCT_NEW(type) \
    STRUCT_NEWREF(type) \
    STRUCT_GC(type) \
    static int type ## _fieldaccess(lua_State *L, int index, int set); \
    STRUCT_INDEX(type) \
    STRUCT_NEWINDEX(type) \
    STRUCT_ACCESS(type) \
    STRUCT_CREATE(type) \
    STRUCT_FIELDACCESS_BEGIN(type)

#define STRUCT_END STRUCT_FIELDACCESS_END

#define STRUCT_NEW(type) \
    int type ## _new(lua_State *L) \
    { \
        return STRUCT_new(L, # type, sizeof(type)); \
    }

#define STRUCT_NEWREF(type) \
    int type ## _newref(lua_State *L, int parentIndex, type *data) \
    { \
        return STRUCT_newref(L, # type, parentIndex, data); \
    }

#define STRUCT_GC(type) \
    static int type ## _gc(lua_State *L) \
    { \
        return STRUCT_gc(L, # type); \
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
    static int STRUCT_access_ ## type(lua_State *L, const char *fieldName, type *data, int parentIndex, int set, int valueIndex) \
    { \
        if (set) { \
            *data = *CHECK_STRUCT(L, valueIndex, type); \
            return 0; \
        } else { \
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
        type *data = CHECK_STRUCT(L, index, type); \
        size_t length = 0; \
        const char *field = STRUCT_fieldname(L, index + 1, &length);

#define STRUCT_FIELD(name, type) \
        if (strncmp(# name, field, length) == 0) { \
            return STRUCT_access_ ## type(L, # name, &data->name, index, set, index + 2); \
        }

#define STRUCT_FIELD_HANDLE(name, type) \
        if (strncmp(# name, field, length) == 0) { \
            return STRUCT_access_handle(L, &data->name, #type, index, set, index + 2); \
        }

#define STRUCT_FIELD_CONSTANT(name, type) \
        if (strncmp(# name, field, length) == 0) { \
            CONSTANT_ACCESS_DECLARE(type); \
            return CONSTANT_access_ ## type(L, &data->name, set, index + 2); \
        }

#define STRUCT_FIELD_CUSTOM(name, ...) \
        if (strncmp(# name, field, length) == 0) { \
            __VA_ARGS__ \
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

STRUCT_BEGIN(FMOD_GUID)
    STRUCT_FIELD(Data1, uint)
    STRUCT_FIELD(Data2, ushort)
    STRUCT_FIELD(Data3, ushort)
    STRUCT_FIELD(Data4, ARRAY_uchar8)
STRUCT_END

STRUCT_BEGIN(FMOD_CREATESOUNDEXINFO)
    STRUCT_FIELD(length, uint)
    STRUCT_FIELD(fileoffset, uint)
    STRUCT_FIELD(numchannels, int)
    STRUCT_FIELD(defaultfrequency, int)
    STRUCT_FIELD_CONSTANT(format, FMOD_SOUND_FORMAT)
    STRUCT_FIELD(decodebuffersize, uint)
    STRUCT_FIELD(initialsubsound, int)
    STRUCT_FIELD(numsubsounds, int)
    // TODO combine these two
    //STRUCT_FIELD_ARRAY_DYNAMIC(inclusionlist, inclusionlistnum, int)
    // TODO FMOD_SOUND_PCMREAD_CALLBACK    pcmreadcallback;
    // TODO FMOD_SOUND_PCMSETPOS_CALLBACK  pcmsetposcallback;
    // TODO FMOD_SOUND_NONBLOCK_CALLBACK   nonblockcallback;
    STRUCT_FIELD(dlsname, cstring)
    STRUCT_FIELD(encryptionkey, cstring)
    STRUCT_FIELD(maxpolyphony, int)
    // TODO void                          *userdata;
    STRUCT_FIELD_CONSTANT(suggestedsoundtype, FMOD_SOUND_TYPE)
    // TODO FMOD_FILE_OPEN_CALLBACK        fileuseropen;
    // TODO FMOD_FILE_CLOSE_CALLBACK       fileuserclose;
    // TODO FMOD_FILE_READ_CALLBACK        fileuserread;
    // TODO FMOD_FILE_SEEK_CALLBACK        fileuserseek;
    // TODO FMOD_FILE_ASYNCREAD_CALLBACK   fileuserasyncread;
    // TODO FMOD_FILE_ASYNCCANCEL_CALLBACK fileuserasynccancel;
    // TODO void                          *fileuserdata;
    STRUCT_FIELD(filebuffersize, int)
    STRUCT_FIELD_CONSTANT(channelorder, FMOD_CHANNELORDER)
    // TODO FMOD_SOUNDGROUP               *initialsoundgroup;
    STRUCT_FIELD(initialseekposition, uint)
    STRUCT_FIELD_CONSTANT(initialseekpostype, FMOD_TIMEUNIT)
    STRUCT_FIELD(ignoresetfilesystem, int)
    STRUCT_FIELD(audioqueuepolicy, uint)
    STRUCT_FIELD(minmidigranularity, uint)
    STRUCT_FIELD(nonblockthreadid, int)
    // TODO FMOD_GUID                     *fsbguid;
STRUCT_END

STRUCT_BEGIN(FMOD_CPU_USAGE)
    STRUCT_FIELD(dsp, float)
    STRUCT_FIELD(stream, float)
    STRUCT_FIELD(geometry, float)
    STRUCT_FIELD(update, float)
    STRUCT_FIELD(convolution1, float)
    STRUCT_FIELD(convolution2, float)
STRUCT_END

STRUCT_BEGIN(FMOD_DSP_PARAMETER_3DATTRIBUTES)
    STRUCT_FIELD(relative, FMOD_3D_ATTRIBUTES)
    STRUCT_FIELD(absolute, FMOD_3D_ATTRIBUTES)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_PARAMETER_ID)
    STRUCT_FIELD(data1, uint)
    STRUCT_FIELD(data2, uint)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_PARAMETER_DESCRIPTION)
    STRUCT_FIELD(name, cstring)
    STRUCT_FIELD(id, FMOD_STUDIO_PARAMETER_ID)
    STRUCT_FIELD(minimum, float)
    STRUCT_FIELD(maximum, float)
    STRUCT_FIELD(defaultvalue, float)
    STRUCT_FIELD_CONSTANT(type, FMOD_STUDIO_PARAMETER_TYPE)
    STRUCT_FIELD_CONSTANT(flags, FMOD_STUDIO_PARAMETER_FLAGS)
    STRUCT_FIELD(guid, FMOD_GUID)
STRUCT_END

static int STRUCT_access_user_property_value(lua_State *L, FMOD_STUDIO_USER_PROPERTY *property, int index, int set,
    int valueIndex)
{
    switch (property->type) {
    case FMOD_STUDIO_USER_PROPERTY_TYPE_INTEGER:
        return STRUCT_access_int(L, "value", &property->intvalue, index, set, index + 2);
    case FMOD_STUDIO_USER_PROPERTY_TYPE_BOOLEAN:
        return STRUCT_access_boolean(L, "value", &property->boolvalue, index, set, index + 2);
    case FMOD_STUDIO_USER_PROPERTY_TYPE_FLOAT:
        return STRUCT_access_float(L, "value", &property->floatvalue, index, set, index + 2);
    case FMOD_STUDIO_USER_PROPERTY_TYPE_STRING:
        return STRUCT_access_cstring(L, "value", &property->stringvalue, index, set, index + 2);
    }

    return -1;
}

STRUCT_BEGIN(FMOD_STUDIO_USER_PROPERTY)
    STRUCT_FIELD(name, cstring)
    STRUCT_FIELD_CONSTANT(type, FMOD_STUDIO_USER_PROPERTY_TYPE)
    STRUCT_FIELD_CUSTOM(value,
        int result = STRUCT_access_user_property_value(L, data, index, set, index + 2);
        if (result >= 0) {
            return result;
        }
    )
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES)
    STRUCT_FIELD(name, cstring)
    STRUCT_FIELD_HANDLE(sound, FMOD_SOUND)
    STRUCT_FIELD(subsoundIndex, int)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_ADVANCEDSETTINGS)
    STRUCT_FIELD(commandqueuesize, uint)
    STRUCT_FIELD(handleinitialsize, uint)
    STRUCT_FIELD(studioupdateperiod, int)
    STRUCT_FIELD(idlesampledatapoolsize, int)
    STRUCT_FIELD(streamingscheduledelay, uint)
    STRUCT_FIELD(encryptionkey, cstring)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_CPU_USAGE)
    STRUCT_FIELD(update, float)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_BUFFER_INFO)
    STRUCT_FIELD(currentusage, int)
    STRUCT_FIELD(peakusage, int)
    STRUCT_FIELD(capacity, int)
    STRUCT_FIELD(stallcount, int)
    STRUCT_FIELD(stalltime, float)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_BUFFER_USAGE)
    STRUCT_FIELD(studiocommandqueue, FMOD_STUDIO_BUFFER_INFO)
    STRUCT_FIELD(studiohandle, FMOD_STUDIO_BUFFER_INFO)
STRUCT_END

STRUCT_BEGIN(luaFMOD_Buffer)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_SOUND_INFO)
    STRUCT_FIELD(name_or_data, luaFMOD_Buffer)
    STRUCT_FIELD_CONSTANT(mode, FMOD_MODE)
    STRUCT_FIELD(exinfo, FMOD_CREATESOUNDEXINFO)
    STRUCT_FIELD(subsoundindex, int)
STRUCT_END

STRUCT_BEGIN(FMOD_STUDIO_MEMORY_USAGE)
    STRUCT_FIELD(exclusive, int)
    STRUCT_FIELD(inclusive, int)
    STRUCT_FIELD(sampledata, int)
STRUCT_END

void createStructTables(lua_State *L)
{
    /* The FMOD table should be on top of the stack, so define FMOD structs first */
    FMOD_VECTOR_create(L, "VECTOR");
    FMOD_3D_ATTRIBUTES_create(L, "_3D_ATTRIBUTES");
    FMOD_GUID_create(L, "GUID");
    FMOD_CREATESOUNDEXINFO_create(L, "CREATESOUNDEXINFO");
    FMOD_CPU_USAGE_create(L, "CPU_USAGE");
    FMOD_DSP_PARAMETER_3DATTRIBUTES_create(L, "DSP_PARAMETER_3DATTRIBUTES");

    /* Get the FMOD.Studio table */
    lua_getfield(L, -1, "Studio");

    /* Define FMOD.Studio structs */
    FMOD_STUDIO_PARAMETER_ID_create(L, "PARAMETER_ID");
    FMOD_STUDIO_PARAMETER_DESCRIPTION_create(L, "PARAMETER_DESCRIPTION");
    FMOD_STUDIO_USER_PROPERTY_create(L, "USER_PROPERTY");
    FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES_create(L, "PROGRAMMER_SOUND_PROPERTIES");
    FMOD_STUDIO_ADVANCEDSETTINGS_create(L, "ADVANCEDSETTINGS");
    FMOD_STUDIO_CPU_USAGE_create(L, "CPU_USAGE");
    FMOD_STUDIO_BUFFER_INFO_create(L, "BUFFER_INFO");
    FMOD_STUDIO_BUFFER_USAGE_create(L, "BUFFER_USAGE");
    FMOD_STUDIO_SOUND_INFO_create(L, "SOUND_INFO");
    FMOD_STUDIO_MEMORY_USAGE_create(L, "MEMORY_USAGE");

    /* Tidy up the FMOD.Studio table */
    lua_pop(L, 1);

    /* Define helper structs */
    ARRAY_uchar8_create(L);
    luaFMOD_Buffer_create(L, NULL);
}
