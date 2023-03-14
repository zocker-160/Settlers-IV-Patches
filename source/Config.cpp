
#include "Config.h"


ConfigData* loadConfig(CSimpleIni& ini) {
	auto* configData = new ConfigData;
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
	configData->bSkipIntro = ini.GetBoolValue("Misc", "SkipIntroVideos");
	configData->bDebugWindow = ini.GetBoolValue("Debug", "DebugWindow");

	return configData;
}
