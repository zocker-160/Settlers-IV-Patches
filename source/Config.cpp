
#include "Config.h"


ConfigData* loadConfig(CSimpleIni& ini) {
	auto* configData = new ConfigData;
	Resolution res = {
		ini.GetLongValue("Resolution", "xRes", 1024),
		ini.GetLongValue("Resolution", "yRes", 768)
	};

	configData->customResolution = res;
	configData->bSoftwareMode = ini.GetBoolValue("Misc", "SoftwareMode");
	configData->bSkipIntro = ini.GetBoolValue("Misc", "SkipIntroVideos");
	configData->bDebugWindow = ini.GetBoolValue("Debug", "DebugWindow");

	return configData;
}
