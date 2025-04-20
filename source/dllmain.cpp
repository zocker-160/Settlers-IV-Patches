
#include "pch.h"

#include "Logger.h"
#include "Helper.h"
#include "SimpleIni.h"

#include "Config.h"
#include "Patch.h"

#define NAKED __declspec(naked) void
#define JMP(addr) {_asm{jmp[addr]}}
#define DLLENTRY(ddraw, func) NAKED f_##func() JMP(ddraw.func)
#define PROXYADDR(ddraw, module, func) ddraw.func = GetProcAddress(module, #func)

struct ddrawDLL {
    FARPROC AcquireDDThreadLock;
    FARPROC CompleteCreateSysmemSurface;
    FARPROC D3DParseUnknownCommand;
    FARPROC DDInternalLock;
    FARPROC DDInternalUnlock;
    FARPROC DirectDrawCreate;
    FARPROC DirectDrawCreateClipper;
    FARPROC DirectDrawCreateEx;
    FARPROC DirectDrawEnumerateA;
    FARPROC DirectDrawEnumerateExA;
    FARPROC DirectDrawEnumerateExW;
    FARPROC DirectDrawEnumerateW;
    FARPROC DllCanUnloadNow;
    FARPROC DllGetClassObject;
    FARPROC ReleaseDDThreadLock;
} ddraw;

DLLENTRY(ddraw, AcquireDDThreadLock)
DLLENTRY(ddraw, CompleteCreateSysmemSurface)
DLLENTRY(ddraw, D3DParseUnknownCommand)
DLLENTRY(ddraw, DDInternalLock)
DLLENTRY(ddraw, DDInternalUnlock)
DLLENTRY(ddraw, DirectDrawCreate)
DLLENTRY(ddraw, DirectDrawCreateClipper)
DLLENTRY(ddraw, DirectDrawCreateEx)
DLLENTRY(ddraw, DirectDrawEnumerateA)
DLLENTRY(ddraw, DirectDrawEnumerateExA)
DLLENTRY(ddraw, DirectDrawEnumerateExW)
DLLENTRY(ddraw, DirectDrawEnumerateW)
DLLENTRY(ddraw, DllCanUnloadNow)
DLLENTRY(ddraw, DllGetClassObject)
DLLENTRY(ddraw, ReleaseDDThreadLock)

void initDLL(HMODULE dll) {
    PROXYADDR(ddraw, dll, AcquireDDThreadLock);
    PROXYADDR(ddraw, dll, CompleteCreateSysmemSurface);
    PROXYADDR(ddraw, dll, D3DParseUnknownCommand);
    PROXYADDR(ddraw, dll, DDInternalLock);
    PROXYADDR(ddraw, dll, DDInternalUnlock);
    PROXYADDR(ddraw, dll, DirectDrawCreate);
    PROXYADDR(ddraw, dll, DirectDrawCreateClipper);
    PROXYADDR(ddraw, dll, DirectDrawCreateEx);
    PROXYADDR(ddraw, dll, DirectDrawEnumerateA);
    PROXYADDR(ddraw, dll, DirectDrawEnumerateExA);
    PROXYADDR(ddraw, dll, DirectDrawEnumerateExW);
    PROXYADDR(ddraw, dll, DirectDrawEnumerateW);
    PROXYADDR(ddraw, dll, DllCanUnloadNow);
    PROXYADDR(ddraw, dll, DllGetClassObject);
    PROXYADDR(ddraw, dll, ReleaseDDThreadLock);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HMODULE baseModule = getBaseModule();

        char mainExecutable[MAX_PATH];
        GetModuleFileNameA(baseModule, mainExecutable, MAX_PATH);

        char configPath[MAX_PATH];
        getGameDirectory(baseModule, configPath, MAX_PATH, "\\WidescreenFix.ini", 0);

        char logPath[MAX_PATH];
        getGameDirectory(baseModule, logPath, MAX_PATH, "\\WidescreenFix.log", 0);

        char videoCFG[MAX_PATH];
        getGameDirectory(baseModule, videoCFG, MAX_PATH, "\\Config\\Video.cfg", 1);


        CSimpleIni config;
        config.SetUnicode();
        config.LoadFile(configPath);

        ConfigData* configData = loadConfig(config);

        Logging::Logger logger("ENTR", logPath, configData->bDebugWindow);
        showHeaderMessage();
        logger.debug() << "log path: " << logPath << std::endl;
        logger.debug() << "config path: " << configPath << std::endl;

        logger.info("Checking game version...");
        if (!isGameVersionSupported(mainExecutable)) {
            logger.error("Unsupported game version - exiting");
            MessageBoxA(NULL, "Unsupported game version!", "WidescreenFix", MB_OK);
            ExitProcess(0);
        }

        logger.info("Applying patches...");
        patchVideoSkip(videoCFG, configData);
        patchResolutions(configData);
        logger.info("Widescreen patches loaded");

        char ddrawPath[MAX_PATH];
        if (configData->bNativeDX || isWine()) {
            logger.debug("Using Native DX");

            GetSystemDirectoryA(ddrawPath, MAX_PATH);
            strcat_s(ddrawPath, "\\ddraw.dll");
        } else {
            getGameDirectory(baseModule, ddrawPath, MAX_PATH, "\\dgVoodoo.dll", 0);
        }

        logger.info() << "Loading " << ddrawPath << std::endl;

        HMODULE dll = LoadLibraryA(ddrawPath);
        if (dll == false) {
            logger.error() << "Failed to load " << ddrawPath << std::endl;
            MessageBoxA(NULL, "DLL load error - check logs", "WidescreenFix", MB_OK);
            ExitProcess(0);
        }

        initDLL(dll);

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        FreeLibrary(hModule);
        break;
    }
    }
    return TRUE;
}
