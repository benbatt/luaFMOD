#ifdef WIN32

#include <lauxlib.h>
#include <windows.h>

#ifdef LUAFMOD_DYNAMIC
static int findBasename(const char *path, DWORD length)
{
    for (int i = length; i >= 0; --i)
    {
        if (path[i] == '\\')
        {
            return i + 1;
        }
    }

    return 0;
}

static void loadFMOD(lua_State *L)
{
    HMODULE module = GetModuleHandleA("luaFMOD");

    void *userdata = NULL;
    lua_Alloc alloc = lua_getallocf(L, &userdata);

    size_t pathCapacity = 32;
    size_t pathSize = 0;
    char *path = NULL;
    size_t basenameIndex = 0;

    const char *fmodDLL = "fmodL.dll";
    const char *fmodstudioDLL = "fmodstudioL.dll";

    int basenameLength = max(strlen(fmodDLL), strlen(fmodstudioDLL));

    while (1)
    {
        pathSize = sizeof(*path) * pathCapacity;
        path = alloc(userdata, NULL, 0, pathSize);

        DWORD pathLength = GetModuleFileNameA(module, path, pathCapacity);

        if (pathLength < pathCapacity)
        {
            basenameIndex = findBasename(path, pathLength);

            if (basenameIndex + basenameLength < pathCapacity)
            {
                break;
            }
        }

        alloc(userdata, path, pathSize, 0);
        pathCapacity *= 2;
    }

#define LOAD_LIBRARY(basename) \
    do { \
        strcpy_s(path + basenameIndex, pathCapacity - basenameIndex, basename); \
        if (!LoadLibraryA(path)) \
        { \
            lua_pushfstring(L, "Failed to load %s", path); \
            alloc(userdata, path, pathSize, 0); \
            lua_error(L); \
        } \
    } while(0)

    LOAD_LIBRARY(fmodDLL);
    LOAD_LIBRARY(fmodstudioDLL);

    alloc(userdata, path, pathSize, 0);
}
#endif


void platformInitialize(lua_State *L)
{
#ifdef LUAFMOD_DYNAMIC
    loadFMOD(L);
#endif

    HRESULT comResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (comResult != S_OK && comResult != S_FALSE)
    {
        luaL_error(L, "Failed to initialize COM");
    }
}

#endif /* WIN32 */
