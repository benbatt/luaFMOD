#ifdef __APPLE__

#include <lauxlib.h>
#include <pthread.h>

#include "../platform.h"

void platformInitialize(lua_State *L)
{
}

#define CHECK(call) \
    do { \
        if ((call) != 0) { \
            return NULL; \
        } \
    } while(0)

LUAFMOD_CRITICAL_SECTION *criticalSectionCreate()
{
    pthread_mutex_t *mutex = malloc(sizeof(*mutex));

    if (!mutex) {
        return NULL;
    }

    pthread_mutexattr_t attr;
    CHECK(pthread_mutexattr_init(&attr));
    CHECK(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));

    CHECK(pthread_mutex_init(mutex, &attr));

    return (LUAFMOD_CRITICAL_SECTION*)mutex;
}

#undef CHECK
#define CHECK(call) \
    do { \
        if ((call) != 0) { \
            return; \
        } \
    } while(0)

void criticalSectionRelease(LUAFMOD_CRITICAL_SECTION *criticalSection)
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)criticalSection;
    CHECK(pthread_mutex_destroy(mutex));

    free(mutex);
}

void criticalSectionEnter(LUAFMOD_CRITICAL_SECTION *criticalSection)
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)criticalSection;
    CHECK(pthread_mutex_lock(mutex));
}

void criticalSectionLeave(LUAFMOD_CRITICAL_SECTION *criticalSection)
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)criticalSection;
    CHECK(pthread_mutex_unlock(mutex));
}

#endif /* __APPLE__ */
