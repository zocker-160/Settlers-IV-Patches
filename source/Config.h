#pragma once

#include "utilities/SimpleIni/SimpleIni.h"

struct Resolution {
	int x;
	int y;
};

struct ConfigData {
	Resolution customResolution;
	bool bSoftwareMode;
	bool bSkipIntro;
	bool bDebugWindow;
};

ConfigData* loadConfig(CSimpleIni& ini);
