#include "pch.h"

#include <stdio.h>

#include "utilities/Helper/Logger.h"
#include "utilities/Helper/Helper.h"
#include "utilities/SimpleIni/SimpleIni.h"

#include "Config.h"
#include "Patch.h"

struct ddraw_dll {
    HMODULE dll;
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

__declspec(naked) void FakeAcquireDDThreadLock() { _asm { jmp[ddraw.AcquireDDThreadLock] } }
__declspec(naked) void FakeCompleteCreateSysmemSurface() { _asm { jmp[ddraw.CompleteCreateSysmemSurface] } }
__declspec(naked) void FakeD3DParseUnknownCommand() { _asm { jmp[ddraw.D3DParseUnknownCommand] } }
__declspec(naked) void FakeDDInternalLock() { _asm { jmp[ddraw.DDInternalLock] } }
__declspec(naked) void FakeDDInternalUnlock() { _asm { jmp[ddraw.DDInternalUnlock] } }
__declspec(naked) void FakeDirectDrawCreate() { _asm { jmp[ddraw.DirectDrawCreate] } }
__declspec(naked) void FakeDirectDrawCreateClipper() { _asm { jmp[ddraw.DirectDrawCreateClipper] } }
__declspec(naked) void FakeDirectDrawCreateEx() { _asm { jmp[ddraw.DirectDrawCreateEx] } }
__declspec(naked) void FakeDirectDrawEnumerateA() { _asm { jmp[ddraw.DirectDrawEnumerateA] } }
__declspec(naked) void FakeDirectDrawEnumerateExA() { _asm { jmp[ddraw.DirectDrawEnumerateExA] } }
__declspec(naked) void FakeDirectDrawEnumerateExW() { _asm { jmp[ddraw.DirectDrawEnumerateExW] } }
__declspec(naked) void FakeDirectDrawEnumerateW() { _asm { jmp[ddraw.DirectDrawEnumerateW] } }
__declspec(naked) void FakeDllCanUnloadNow() { _asm { jmp[ddraw.DllCanUnloadNow] } }
__declspec(naked) void FakeDllGetClassObject() { _asm { jmp[ddraw.DllGetClassObject] } }
__declspec(naked) void FakeReleaseDDThreadLock() { _asm { jmp[ddraw.ReleaseDDThreadLock] } }


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CSimpleIni config;

        char ddrawPath[MAX_PATH];
        getGameDirectory(hModule, ddrawPath, MAX_PATH, "\\dgVoodoo.dll", 0);

        char configPath[MAX_PATH];
        getGameDirectory(hModule, configPath, MAX_PATH, "\\WidescreenFix.ini", 0);

        char logPath[MAX_PATH];
        getGameDirectory(hModule, logPath, MAX_PATH, "\\WidescreenFix.log", 0);

        char videoCFG[MAX_PATH];
        getGameDirectory(hModule, videoCFG, MAX_PATH, "\\Config\\Video.cfg", 1);

        config.SetUnicode();
        config.LoadFile(configPath);

        ConfigData* configData = loadConfig(config);

        Logging::Logger logger("ENTR", logPath, configData->bDebugWindow);
        showHeaderMessage();
        logger.debug() << "log path: " << logPath << std::endl;
        logger.debug() << "config path: " << configPath << std::endl;

        logger.info("Widescreen Patch loaded");

        patchVideoSkip(videoCFG, configData);
        patchResolutions(configData);

        logger.info("Patching done!");

        logger.info("Loading ddraw...");

        if (isWine()) {
            logger.debug("WINE detected");
            ddraw.dll = LoadLibrary(ddrawPath);
        }
        else {
            ddraw.dll = LoadLibrary(ddrawPath);
        }

        if (ddraw.dll == false) {
            logger.error() << "Failed to load " << ddrawPath << std::endl;
            MessageBoxA(NULL, "Failed to load dgVoodoo", "WidescreenFix", MB_ICONERROR);
            ExitProcess(0);
        }

        ddraw.AcquireDDThreadLock = GetProcAddress(ddraw.dll, "AcquireDDThreadLock");
        ddraw.CompleteCreateSysmemSurface = GetProcAddress(ddraw.dll, "CompleteCreateSysmemSurface");
        ddraw.D3DParseUnknownCommand = GetProcAddress(ddraw.dll, "D3DParseUnknownCommand");
        ddraw.DDInternalLock = GetProcAddress(ddraw.dll, "DDInternalLock");
        ddraw.DDInternalUnlock = GetProcAddress(ddraw.dll, "DDInternalUnlock");
        ddraw.DirectDrawCreate = GetProcAddress(ddraw.dll, "DirectDrawCreate");
        ddraw.DirectDrawCreateClipper = GetProcAddress(ddraw.dll, "DirectDrawCreateClipper");
        ddraw.DirectDrawCreateEx = GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
        ddraw.DirectDrawEnumerateA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateA");
        ddraw.DirectDrawEnumerateExA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExA");
        ddraw.DirectDrawEnumerateExW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExW");
        ddraw.DirectDrawEnumerateW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateW");
        ddraw.DllCanUnloadNow = GetProcAddress(ddraw.dll, "DllCanUnloadNow");
        ddraw.DllGetClassObject = GetProcAddress(ddraw.dll, "DllGetClassObject");
        ddraw.ReleaseDDThreadLock = GetProcAddress(ddraw.dll, "ReleaseDDThreadLock");

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
