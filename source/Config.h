#pragma once

#include "utilities/SimpleIni/SimpleIni.h"

struct Resolution {
	int x;
	int y;
};

struct ConfigData {
	Resolution customResolution2;
	Resolution customResolution3;
	bool bCustomRes3;
	bool bSoftwareMode;
	bool bSkipIntro;
	bool bDebugWindow;
	bool bNativeDX;
};

ConfigData* loadConfig(CSimpleIni& ini);
