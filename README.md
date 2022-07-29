Building as a DLL Module on Windows
-----------------------------------

1. Install [Lua for Windows](https://github.com/rjpcomputing/luaforwindows)
2. Install the FMOD Engine Windows package from [the FMOD download page](www.fmod.com/download)
3. Copy headers and libraries from the FMOD API directory (defaults to `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api`) into the source tree:
    * Copy all .h files from `<FMOD API>\core\inc` into `external\FMOD\inc`
    * Copy all .h files from `<FMOD API>\studio\inc` into `external\FMOD\inc`
    * Copy all .dll and .lib files from `<FMOD API>\core\lib\x86` into `external\FMOD\lib`
    * Copy all .dll and .lib files from `<FMOD API>\studio\lib\x86` into `external\FMOD\lib`
4. Open `build\windows\vs2019\luaFMOD.sln` in Visual Studio 2019
5. Select Build Solution from the Build menu
