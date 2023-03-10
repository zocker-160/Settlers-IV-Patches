#pragma once

#include "utilities/SimpleIni/SimpleIni.h"

struct Resolution {
	int x;
	int y;
};

struct ConfigData {
	Resolution resMode2;
	Resolution resMode3;
	bool bSkipIntro;
	bool bDebugWindow;
};

ConfigData* loadConfig(CSimpleIni& ini);
