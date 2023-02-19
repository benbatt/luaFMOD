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
#include "logging.h"
#include <stdlib.h>

#define SELF_TYPE FMOD_STUDIO_SYSTEM

static int create(lua_State *L)
{
    FMOD_STUDIO_SYSTEM *system = NULL;

    REQUIRE_OK(FMOD_Studio_System_Create(&system, FMOD_VERSION));

    PUSH_HANDLE(L, FMOD_STUDIO_SYSTEM, system);

    return 1;
}

static int setAdvancedSettings(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_ADVANCEDSETTINGS *settings = CHECK_STRUCT(L, 2, FMOD_STUDIO_ADVANCEDSETTINGS);
    settings->cbsize = sizeof(*settings);

    RETURN_STATUS(FMOD_Studio_System_SetAdvancedSettings(self, settings));
}

static int getAdvancedSettings(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_ADVANCEDSETTINGS settings;
    settings.cbsize = sizeof(settings);

    RETURN_IF_ERROR(FMOD_Studio_System_GetAdvancedSettings(self, &settings));

    PUSH_STRUCT(L, FMOD_STUDIO_ADVANCEDSETTINGS, settings);

    return 1;
}

static int initialize(lua_State *L)
{
    GET_SELF;

    int maxchannels = luaL_checkint(L, 2);
    int studioflags = CHECK_CONSTANT(L, 3, FMOD_STUDIO_INITFLAGS);
    int coreflags = CHECK_CONSTANT(L, 4, FMOD_INITFLAGS);

    /* TODO extradriverdata? */

    REQUIRE_OK(FMOD_Studio_System_Initialize(self, maxchannels, studioflags, coreflags, NULL));

    return 0;
}

static int release(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_Studio_System_Release(self));

    return 0;
}

static int update(lua_State *L)
{
    GET_SELF;

    REQUIRE_OK(FMOD_Studio_System_Update(self));

    loggingPumpMessages(L);

    return 0;
}

static int getCoreSystem(lua_State *L)
{
    GET_SELF;

    FMOD_SYSTEM *system = NULL;
    REQUIRE_OK(FMOD_Studio_System_GetCoreSystem(self, &system));

    PUSH_HANDLE(L, FMOD_SYSTEM, system);

    return 1;
}

static int getEvent(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetEvent(self, path, &description));

    PUSH_HANDLE(L, FMOD_STUDIO_EVENTDESCRIPTION, description);

    return 1;
}

static int getBus(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_BUS *bus = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBus(self, path, &bus));

    PUSH_HANDLE(L, FMOD_STUDIO_BUS, bus);

    return 1;
}

static int getVCA(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_VCA *vca = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetVCA(self, path, &vca));

    PUSH_HANDLE(L, FMOD_STUDIO_VCA, vca);

    return 1;
}

static int getBank(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBank(self, path, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

static int getEventByID(lua_State *L)
{
    GET_SELF;

    const FMOD_GUID *id = CHECK_STRUCT(L, 2, FMOD_GUID);

    FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetEventByID(self, id, &description));

    PUSH_HANDLE(L, FMOD_STUDIO_EVENTDESCRIPTION, description);

    return 1;
}

static int getBusByID(lua_State *L)
{
    GET_SELF;

    const FMOD_GUID *id = CHECK_STRUCT(L, 2, FMOD_GUID);

    FMOD_STUDIO_BUS *bus = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBusByID(self, id, &bus));

    PUSH_HANDLE(L, FMOD_STUDIO_BUS, bus);

    return 1;
}

static int getVCAByID(lua_State *L)
{
    GET_SELF;

    const FMOD_GUID *id = CHECK_STRUCT(L, 2, FMOD_GUID);

    FMOD_STUDIO_VCA *vca = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetVCAByID(self, id, &vca));

    PUSH_HANDLE(L, FMOD_STUDIO_VCA, vca);

    return 1;
}

static int getBankByID(lua_State *L)
{
    GET_SELF;

    const FMOD_GUID *id = CHECK_STRUCT(L, 2, FMOD_GUID);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBankByID(self, id, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

static int getSoundInfo(lua_State *L)
{
    GET_SELF;

    const char *key = luaL_checkstring(L, 2);

    FMOD_STUDIO_SOUND_INFO info;
    RETURN_IF_ERROR(FMOD_Studio_System_GetSoundInfo(self, key, &info));

    PUSH_STRUCT(L, FMOD_STUDIO_SOUND_INFO, info);

    return 1;
}

static int getParameterDescriptionByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    FMOD_STUDIO_PARAMETER_DESCRIPTION description;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterDescriptionByName(self, name, &description));

    PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, description);

    return 1;
}

static int getParameterDescriptionByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);

    FMOD_STUDIO_PARAMETER_DESCRIPTION description;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterDescriptionByID(self, *id, &description));

    PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, description);

    return 1;
}

static int getParameterLabelByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    int labelindex = luaL_checkint(L, 3);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterLabelByName(self, name, labelindex, NULL, 0, &size));

    char *label = malloc(size);

    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterLabelByName(self, name, labelindex, label, size, NULL));

    lua_pushstring(L, label);

    free(label);

    return 1;
}

static int getParameterLabelByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    int labelindex = luaL_checkint(L, 3);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterLabelByID(self, *id, labelindex, NULL, 0, &size));

    char *label = malloc(size);

    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterLabelByID(self, *id, labelindex, label, size, NULL));

    lua_pushstring(L, label);

    free(label);

    return 1;
}

static int getParameterByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);

    float value = 0;
    float finalvalue = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterByID(self, *id, &value, &finalvalue));

    lua_pushnumber(L, value);
    lua_pushnumber(L, finalvalue);

    return 2;
}

static int setParameterByID(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_System_SetParameterByID(self, *id, value, ignoreseekspeed));
}

static int setParameterByIDWithLabel(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_PARAMETER_ID *id = CHECK_STRUCT(L, 2, FMOD_STUDIO_PARAMETER_ID);
    const char *label = luaL_checkstring(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_System_SetParameterByIDWithLabel(self, *id, label, ignoreseekspeed));
}

static int setParametersByIDs(lua_State *L)
{
    GET_SELF;

    const int IDS_INDEX = 2;
    const int VALUES_INDEX = 3;

    luaL_checktype(L, IDS_INDEX, LUA_TTABLE);
    luaL_checktype(L, VALUES_INDEX, LUA_TTABLE);

    int idCount = lua_objlen(L, IDS_INDEX);
    luaL_argcheck(L, idCount > 0, IDS_INDEX, "expected at least one ID");

    int valueCount = lua_objlen(L, VALUES_INDEX);
    luaL_argcheck(L, valueCount > 0, VALUES_INDEX, "expected at least one value");

    int count = (idCount < valueCount) ? idCount : valueCount;

    for (int i = 1; i <= count; ++i) {
        lua_pushnumber(L, i);
        lua_gettable(L, IDS_INDEX);

        if (!IS_STRUCT(L, -1, FMOD_STUDIO_PARAMETER_ID)) {
            return luaL_error(L, "ID list element %d is not a PARAMETER_ID", i);
        }

        lua_pop(L, 1);

        lua_pushnumber(L, i);
        lua_gettable(L, VALUES_INDEX);

        if (!lua_isnumber(L, -1)) {
            return luaL_error(L, "value list element %d is not a number", i);
        }

        lua_pop(L, 1);
    }

    FMOD_STUDIO_PARAMETER_ID *ids = malloc(sizeof(*ids) * count);
    float *values = malloc(sizeof(*values) * count);

    if (!ids || !values) {
        return luaL_error(L, "Out of memory");
    }

    for (int i = 1; i <= count; ++i) {
        lua_pushnumber(L, i);
        lua_gettable(L, IDS_INDEX);
        ids[i - 1] = *CHECK_STRUCT(L, -1, FMOD_STUDIO_PARAMETER_ID);
        lua_pop(L, 1);

        lua_pushnumber(L, i);
        lua_gettable(L, VALUES_INDEX);
        values[i - 1] = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_IF_ERROR(FMOD_Studio_System_SetParametersByIDs(self, ids, values, count, ignoreseekspeed));

    free(values);
    free(ids);

    RETURN_STATUS(FMOD_OK);
}

static int getParameterByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);

    float value = 0;
    float finalvalue = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterByName(self, name, &value, &finalvalue));

    lua_pushnumber(L, value);
    lua_pushnumber(L, finalvalue);

    return 2;
}

static int setParameterByName(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    float value = (float)luaL_checknumber(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_System_SetParameterByName(self, name, value, ignoreseekspeed));
}

static int setParameterByNameWithLabel(lua_State *L)
{
    GET_SELF;

    const char *name = luaL_checkstring(L, 2);
    const char *label = luaL_checkstring(L, 3);
    int ignoreseekspeed = lua_toboolean(L, 4);

    RETURN_STATUS(FMOD_Studio_System_SetParameterByNameWithLabel(self, name, label, ignoreseekspeed));
}

static int lookupID(lua_State *L)
{
    GET_SELF;

    const char *path = luaL_checkstring(L, 2);

    FMOD_GUID id;
    RETURN_IF_ERROR(FMOD_Studio_System_LookupID(self, path, &id));

    PUSH_STRUCT(L, FMOD_GUID, id);

    return 1;
}

static int lookupPath(lua_State *L)
{
    GET_SELF;

    const FMOD_GUID *id = CHECK_STRUCT(L, 2, FMOD_GUID);

    int size = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_LookupPath(self, id, NULL, 0, &size));

    STACKBUFFER_CREATE(char, path, size);

    RETURN_IF_ERROR(FMOD_Studio_System_LookupPath(self, id, path, size, NULL), STACKBUFFER_RELEASE(path););

    lua_pushstring(L, path);

    STACKBUFFER_RELEASE(path);

    return 1;
}

static int getNumListeners(lua_State *L)
{
    GET_SELF;

    int numListeners = 0;

    RETURN_IF_ERROR(FMOD_Studio_System_GetNumListeners(self, &numListeners));

    lua_pushinteger(L, numListeners);

    return 1;
}

static int setNumListeners(lua_State *L)
{
    GET_SELF;

    int numListeners = luaL_checkint(L, 2);

    RETURN_STATUS(FMOD_Studio_System_SetNumListeners(self, numListeners));
}

static int getListenerAttributes(lua_State *L)
{
    GET_SELF;

    int listener = luaL_checkint(L, 2);

    FMOD_3D_ATTRIBUTES attributes;
    FMOD_VECTOR attenuationposition;
    RETURN_IF_ERROR(FMOD_Studio_System_GetListenerAttributes(self, listener, &attributes, &attenuationposition));

    PUSH_STRUCT(L, FMOD_3D_ATTRIBUTES, attributes);
    PUSH_STRUCT(L, FMOD_VECTOR, attenuationposition);

    return 2;
}

static int setListenerAttributes(lua_State *L)
{
    GET_SELF;

    int listener = luaL_checkint(L, 2);

    FMOD_3D_ATTRIBUTES *attributes = CHECK_STRUCT(L, 3, FMOD_3D_ATTRIBUTES);
    FMOD_VECTOR *attenuationposition = OPTIONAL_STRUCT(L, 4, FMOD_VECTOR);

    RETURN_STATUS(FMOD_Studio_System_SetListenerAttributes(self, listener, attributes, attenuationposition));
}

static int getListenerWeight(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkint(L, 2);

    float weight = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetListenerWeight(self, index, &weight));

    lua_pushnumber(L, weight);

    return 1;
}

static int setListenerWeight(lua_State *L)
{
    GET_SELF;

    int index = luaL_checkint(L, 2);
    float weight = (float)luaL_checknumber(L, 3);

    RETURN_STATUS(FMOD_Studio_System_SetListenerWeight(self, index, weight));
}

static int loadBankFile(lua_State *L)
{
    GET_SELF;

    const char *filename = luaL_checkstring(L, 2);
    int flags = CHECK_CONSTANT(L, 3, FMOD_STUDIO_LOAD_BANK_FLAGS);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_LoadBankFile(self, filename, flags, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

static int loadBankMemory(lua_State *L)
{
    GET_SELF;

    if (lua_type(L, 2) != LUA_TSTRING) {
        return luaL_error(L, "buffer argument must be a string");
    }

    size_t length = 0;
    const char *buffer = lua_tolstring(L, 2, &length);

    int mode = CHECK_CONSTANT(L, 3, FMOD_STUDIO_LOAD_MEMORY_MODE);
    int flags = CHECK_CONSTANT(L, 4, FMOD_STUDIO_LOAD_BANK_FLAGS);

    FMOD_STUDIO_BANK *bank = NULL;
    RETURN_IF_ERROR(FMOD_Studio_System_LoadBankMemory(self, buffer, length, mode, flags, &bank));

    PUSH_HANDLE(L, FMOD_STUDIO_BANK, bank);

    return 1;
}

static int unloadAll(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_System_UnloadAll(self));
}

static int flushCommands(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_System_FlushCommands(self));
}

static int flushSampleLoading(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_System_FlushSampleLoading(self));
}

static int startCommandCapture(lua_State *L)
{
    GET_SELF;

    const char *filename = luaL_checkstring(L, 2);
    int flags = CHECK_CONSTANT(L, 3, FMOD_STUDIO_COMMANDCAPTURE_FLAGS);

    RETURN_STATUS(FMOD_Studio_System_StartCommandCapture(self, filename, flags));
}

static int stopCommandCapture(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_System_StopCommandCapture(self));
}

static int getBankCount(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBankCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int getBankList(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBankCount(self, &count));

    if (count == 0) {
        lua_newtable(L);
        return 1;
    }

    STACKBUFFER_CREATE(FMOD_STUDIO_BANK*, array, count);

    RETURN_IF_ERROR(FMOD_Studio_System_GetBankList(self, array, count, &count), STACKBUFFER_RELEASE(array););

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i) {
        PUSH_HANDLE(L, FMOD_STUDIO_BANK, array[i]);
        lua_rawseti(L, -2, i + 1);
    }

    STACKBUFFER_RELEASE(array);

    return 1;
}

static int getParameterDescriptionCount(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterDescriptionCount(self, &count));

    lua_pushinteger(L, count);

    return 1;
}

static int getParameterDescriptionList(lua_State *L)
{
    GET_SELF;

    int count = 0;
    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterDescriptionCount(self, &count));

    if (count == 0) {
        lua_newtable(L);
        return 1;
    }

    STACKBUFFER_CREATE(FMOD_STUDIO_PARAMETER_DESCRIPTION, array, count);

    RETURN_IF_ERROR(FMOD_Studio_System_GetParameterDescriptionList(self, array, count, &count),
        STACKBUFFER_RELEASE(array););

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i) {
        PUSH_STRUCT(L, FMOD_STUDIO_PARAMETER_DESCRIPTION, array[i]);
        lua_rawseti(L, -2, i + 1);
    }

    STACKBUFFER_RELEASE(array);

    return 1;
}

static int getCPUUsage(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_CPU_USAGE usage;
    FMOD_CPU_USAGE usage_core;
    RETURN_IF_ERROR(FMOD_Studio_System_GetCPUUsage(self, &usage, &usage_core));

    PUSH_STRUCT(L, FMOD_STUDIO_CPU_USAGE, usage);
    PUSH_STRUCT(L, FMOD_CPU_USAGE, usage_core);

    return 2;
}

static int getBufferUsage(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_BUFFER_USAGE usage;
    RETURN_IF_ERROR(FMOD_Studio_System_GetBufferUsage(self, &usage));

    PUSH_STRUCT(L, FMOD_STUDIO_BUFFER_USAGE, usage);

    return 1;
}

static int resetBufferUsage(lua_State *L)
{
    GET_SELF;

    RETURN_STATUS(FMOD_Studio_System_ResetBufferUsage(self));
}

static int getMemoryUsage(lua_State *L)
{
    GET_SELF;

    FMOD_STUDIO_MEMORY_USAGE usage;
    RETURN_IF_ERROR(FMOD_Studio_System_GetMemoryUsage(self, &usage));

    PUSH_STRUCT(L, FMOD_STUDIO_MEMORY_USAGE, usage);

    return 1;
}

FUNCTION_TABLE_BEGIN(StudioSystemStaticFunctions)
    FUNCTION_TABLE_ENTRY(create)
FUNCTION_TABLE_END

METHODS_TABLE_BEGIN
    METHODS_TABLE_ENTRY(setAdvancedSettings)
    METHODS_TABLE_ENTRY(getAdvancedSettings)
    METHODS_TABLE_ENTRY(initialize)
    METHODS_TABLE_ENTRY(release)
    METHODS_TABLE_ENTRY(update)
    METHODS_TABLE_ENTRY(getCoreSystem)
    METHODS_TABLE_ENTRY(getEvent)
    METHODS_TABLE_ENTRY(getBus)
    METHODS_TABLE_ENTRY(getVCA)
    METHODS_TABLE_ENTRY(getBank)
    METHODS_TABLE_ENTRY(getEventByID)
    METHODS_TABLE_ENTRY(getBusByID)
    METHODS_TABLE_ENTRY(getVCAByID)
    METHODS_TABLE_ENTRY(getBankByID)
    METHODS_TABLE_ENTRY(getSoundInfo)
    METHODS_TABLE_ENTRY(getParameterDescriptionByName)
    METHODS_TABLE_ENTRY(getParameterDescriptionByID)
    METHODS_TABLE_ENTRY(getParameterLabelByName)
    METHODS_TABLE_ENTRY(getParameterLabelByID)
    METHODS_TABLE_ENTRY(getParameterByID)
    METHODS_TABLE_ENTRY(setParameterByID)
    METHODS_TABLE_ENTRY(setParameterByIDWithLabel)
    METHODS_TABLE_ENTRY(setParametersByIDs)
    METHODS_TABLE_ENTRY(getParameterByName)
    METHODS_TABLE_ENTRY(setParameterByName)
    METHODS_TABLE_ENTRY(setParameterByNameWithLabel)
    METHODS_TABLE_ENTRY(lookupID)
    METHODS_TABLE_ENTRY(lookupPath)
    METHODS_TABLE_ENTRY(getNumListeners)
    METHODS_TABLE_ENTRY(setNumListeners)
    METHODS_TABLE_ENTRY(getListenerAttributes)
    METHODS_TABLE_ENTRY(setListenerAttributes)
    METHODS_TABLE_ENTRY(getListenerWeight)
    METHODS_TABLE_ENTRY(setListenerWeight)
    METHODS_TABLE_ENTRY(loadBankFile)
    METHODS_TABLE_ENTRY(loadBankMemory)
    METHODS_TABLE_ENTRY(unloadAll)
    METHODS_TABLE_ENTRY(flushCommands)
    METHODS_TABLE_ENTRY(flushSampleLoading)
    METHODS_TABLE_ENTRY(startCommandCapture)
    METHODS_TABLE_ENTRY(stopCommandCapture)
    METHODS_TABLE_ENTRY(getBankCount)
    METHODS_TABLE_ENTRY(getBankList)
    METHODS_TABLE_ENTRY(getParameterDescriptionCount)
    METHODS_TABLE_ENTRY(getParameterDescriptionList)
    METHODS_TABLE_ENTRY(getCPUUsage)
    METHODS_TABLE_ENTRY(getBufferUsage)
    METHODS_TABLE_ENTRY(resetBufferUsage)
    METHODS_TABLE_ENTRY(getMemoryUsage)
METHODS_TABLE_END
