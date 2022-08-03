#ifndef LOGGING_H
#define LOGGING_H

#include <fmod_common.h>
#include <lauxlib.h>

FMOD_RESULT loggingStart(FMOD_DEBUG_FLAGS flags, FMOD_DEBUG_MODE mode, int luaCallback, const char *filename);
void loggingPumpMessages(lua_State *L);

#endif /* LOGGING_H */
