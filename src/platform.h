#ifndef PLATFORM_H
#define PLATFORM_H

/* These are defined in platforms/<platform>.c */
void platformInitialize(struct lua_State *L);

typedef struct LUAFMOD_CRITICAL_SECTION LUAFMOD_CRITICAL_SECTION;

LUAFMOD_CRITICAL_SECTION *criticalSectionCreate();
void criticalSectionRelease(LUAFMOD_CRITICAL_SECTION *criticalSection);
void criticalSectionEnter(LUAFMOD_CRITICAL_SECTION *criticalSection);
void criticalSectionLeave(LUAFMOD_CRITICAL_SECTION *criticalSection);

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
