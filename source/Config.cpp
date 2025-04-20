
#include "Config.h"
#include "Helper.h"

#include <stdio.h>

ConfigData* loadConfig(CSimpleIni& ini) {
	auto configData = new ConfigData;
	Resolution res2 = {
		ini.GetLongValue("Resolution", "xRes", 1024),
		ini.GetLongValue("Resolution", "yRes", 768)
	};
	Resolution res3 = {
		ini.GetLongValue("Resolution3", "xRes", 1280),
		ini.GetLongValue("Resolution3", "yRes", 1024)
	};

	configData->customResolution2 = res2;
	configData->customResolution3 = res3;
	configData->bCustomRes3 = ini.GetBoolValue("Resolution3", "enabled");
	configData->bSoftwareMode = ini.GetBoolValue("Misc", "SoftwareMode");
	configData->bSkipIntro = ini.GetBoolValue("Misc", "SkipVideos");
	configData->bNativeDX = ini.GetBoolValue("Misc", "UseNativeDX");
	configData->bDebugWindow = ini.GetBoolValue("Debug", "DebugWindow");

	return configData;
}

bool isGameVersionSupported(char *executable) {
	std::string checksum;

	if (!getFileChecksum(executable, checksum)) {
		return false;
	}

	return checksum == "edf54528e2bb9e5c757918f44026c1edb3f092abc9af0957a2cc2330971a17bb" 
		|| checksum == "4b1670e105a2e75da73f6248d8ac88264543c5480b3fa19066a3d61d95ce29b1";
}
