#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <fmod_studio_common.h>
#include <lauxlib.h>

int callbackPrepare(lua_State *L, int index, void *owner);

int callbacks_checkUserData(lua_State *L, int index);

int callbacks_getUserData(lua_State *L, void *owner);
int callbacks_setUserData(lua_State *L, int index, void *owner);

FMOD_RESULT F_CALLBACK eventCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event,
    void *parameters);

#endif /* CALLBACKS_H */
