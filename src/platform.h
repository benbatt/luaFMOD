#ifndef PLATFORM_H
#define PLATFORM_H

/* This is defined in platforms/<platform>.c */
extern void platformInitialize(struct lua_State *L);

#ifdef LUAFMOD_DYNAMIC
    #ifdef _WIN32
        #define LUAFMOD_EXPORT __declspec(dllexport)
    #else
        #define LUAFMOD_EXPORT
    #endif
#else
    #define LUAFMOD_EXPORT
#endif

#endif /* PLATFORM_H */
