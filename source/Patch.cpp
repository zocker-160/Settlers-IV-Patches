
#include "pch.h"

#include "utilities/Helper/Helper.h"
#include "utilities/Helper/Logger.h"

namespace Patch_Logger {
	Logging::Logger logger("MAIN");
}
using Patch_Logger::logger;

#include "Patch.h"

DWORD mode0X = 0x1068E-7-5-7-5;
DWORD mode0Y = 0x1068E-7-5-7;

DWORD mode1X = 0x1068E-7-5;
DWORD mode1Y = 0x1068E-7;

DWORD mode2X = 0x1068E;
DWORD mode2Y = 0x1068E+5;

DWORD mode3X = 0x1068E+5+7;
DWORD mode3Y = 0x1068E+5+7+5;

DWORD mode4X = 0x1068E+5+7+5+7;
DWORD mode4Y = 0x1068E+5+7+5+7+5;

DWORD modeFallbackX;
DWORD modeFallbackY;


HMODULE getGFXAddress() {
	return getModuleAddress("GfxEngine.dll");
}

void writeResolutionTuple(const char* name, Resolution res, DWORD res1, DWORD res2) {
	int* xres = (int*)calcModuleAddress(getGFXAddress(), res1);
	int* yres = (int*)calcModuleAddress(getGFXAddress(), res2);

	logger.debug() << "Current " << name << " resolution: " 
		<< *xres << "x" << *yres << std::endl;

	writeBytes(xres, &res.x, 4);
	writeBytes(yres, &res.y, 4);

	logger.debug() << "NEW " << name << " resolution: " 
		<< *xres << "x" << *yres << std::endl;
}

void patchResolutions(ConfigData* cf) {
	logger.info("starting resolution patch...");

	writeResolutionTuple("Res2", cf->resMode2, mode2X, mode2Y);
	writeResolutionTuple("Res3", cf->resMode3, mode3X, mode3Y);

	logger.info("patching done");
}
