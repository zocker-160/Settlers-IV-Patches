#include "pch.h"

#include <stdio.h>

#include "utilities/Helper/Logger.h"
#include "utilities/Helper/Helper.h"
#include "utilities/SimpleIni/SimpleIni.h"

#include "Config.h"
#include "Patch.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CSimpleIni config;

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
