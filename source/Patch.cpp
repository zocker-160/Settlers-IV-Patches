
#include "pch.h"

#include <fstream>

#include "utilities/Helper/Helper.h"
#include "utilities/Helper/Logger.h"

namespace Patch_Logger {
	Logging::Logger logger("MAIN");
}
using Patch_Logger::logger;

#include "Patch.h"

namespace GfxEngine {
	DWORD mode0X = 0x1068E - 7 - 5 - 7 - 5;
	DWORD mode0Y = 0x1068E - 7 - 5 - 7;

	DWORD mode1X = 0x1068E - 7 - 5;
	DWORD mode1Y = 0x1068E - 7;

	DWORD mode2X = 0x1068E;
	DWORD mode2Y = 0x1068E + 5;

	DWORD mode3X = 0x1068E + 5 + 7;
	DWORD mode3Y = 0x1068E + 5 + 7 + 5;

	DWORD mode4X = 0x1068E + 5 + 7 + 5 + 7;
	DWORD mode4Y = 0x1068E + 5 + 7 + 5 + 7 + 5;

	DWORD modeFallbackX;
	DWORD modeFallbackY;
}

namespace S4 {
	DWORD mode0X = 0x265B40;
	DWORD mode0Y = 0x265B40 + 0x4;

	DWORD mode1X = 0x265B40 + 0x8;
	DWORD mode1Y = 0x265B40 + 0xC;

	DWORD mode2X = 0x265B40 + 0x10;
	DWORD mode2Y = 0x265B40 + 0x14;

	DWORD mode3X = 0x265B40 + 0x18;
	DWORD mode3Y = 0x265B40 + 0x1C;

	DWORD mode4X = 0x265B40 + 0x20;
	DWORD mode4Y = 0x265B40 + 0x24;
}

HMODULE getGFXAddress() {
	return getModuleAddress("GfxEngine.dll");
}

void writeResolutionTuple(const char* name, HMODULE module, Resolution res, DWORD res1, DWORD res2) {
	int* xres = (int*)calcModuleAddress(module, res1);
	int* yres = (int*)calcModuleAddress(module, res2);

	logger.debug() << name << ": " << *xres << " x " << *yres;

	writeBytes(xres, &res.x, 4);
	writeBytes(yres, &res.y, 4);

	logger.naked() << " -> " << *xres << " x " << *yres << std::endl;
}

void patchResolutions(ConfigData* cf) {
	logger.info("resolution patch...");

	writeResolutionTuple("Res Mode 2 GFX", getGFXAddress(),
		cf->customResolution, GfxEngine::mode2X, GfxEngine::mode2Y);
	writeResolutionTuple("Res Mode 2 EXE", getBaseAddress(),
		cf->customResolution, S4::mode2X, S4::mode2Y);

	// set Res Mode 3 to native desktop resolution
	int hor, ver;
	getDesktopResolution2(hor, ver);
	Resolution desktopRes = { hor, ver };

	writeResolutionTuple("Res Mode 3 GFX", getGFXAddress(),
		desktopRes, GfxEngine::mode3X, GfxEngine::mode3Y);
	writeResolutionTuple("Res Mode 3 EXE", getBaseAddress(),
		desktopRes, S4::mode3X, S4::mode3Y);

	logger.info("resolution patch done");
}

void patchVideoSkip(char* path, ConfigData* cf) {
	logger.info("video skip patch...");

	remove(path);
	std::ofstream f(path);

	f << "[ADVGAMESETTINGS]" << std::endl;
	f << "{" << std::endl;
	f << "ForceBlit = 0" << std::endl;
	f << "PureSoftwareMode = " << (cf->bSoftwareMode ? 1 : 0) << std::endl;
	f << "ShowVideos = " << (cf->bSkipIntro ? 0 : 1) << std::endl;
	f << "}";

	f.close();

	logger.info("video skip patch done");
}
