
#include "Config.h"


ConfigData* loadConfig(CSimpleIni& ini) {
	auto* configData = new ConfigData;
	Resolution resMode2 = {
		ini.GetLongValue("ResMode2", "xRes", 1024),
		ini.GetLongValue("ResMode2", "yRes", 768)
	};
	Resolution resMode3 = {
		ini.GetLongValue("ResMode3", "xRes", 1280),
		ini.GetLongValue("ResMode3", "yRes", 1024)
	};

	configData->resMode2 = resMode2;
	configData->resMode3 = resMode3;
	configData->bSkipIntro = ini.GetBoolValue("Misc", "SkipIntroVideos");
	configData->bDebugWindow = ini.GetBoolValue("Debug", "DebugWindow");

	return configData;
}
