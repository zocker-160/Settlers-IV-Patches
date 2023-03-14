
#include "pch.h"

#include <fstream>
#include <sstream>
#include <stdio.h>

#include "utilities/Helper/Helper.h"
#include "utilities/Helper/Logger.h"

namespace Patch_Logger {
	Logging::Logger logger("MAIN");
	Logging::Logger guiLogger("S4 GUI");
}
using Patch_Logger::logger;
using Patch_Logger::guiLogger;

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

DWORD GuiEngine_SetText = 0x26C0;

HMODULE getGFXAddress() {
	return getModuleAddress("GfxEngine.dll");
}

HMODULE getGuiAddress() {
	return getModuleAddress("GuiEngine2.dll");
}

void showHeaderMessage() {
	logger.naked() << "Widescreen Patch for Settlers IV Gold by zocker_160 v"
		<< ver_maj << "." << ver_min << "\n" << std::endl;
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
		cf->customResolution2, GfxEngine::mode2X, GfxEngine::mode2Y);
	writeResolutionTuple("Res Mode 2 EXE", getBaseAddress(),
		cf->customResolution2, S4::mode2X, S4::mode2Y);

	// set Res Mode 3
	int hor, ver;
	if (cf->bCustomRes3) {
		logger.debug("Custom resolution 3 override enabled");

		hor = cf->customResolution3.x;
		ver = cf->customResolution3.y;
	}
	else {
		getDesktopResolution2(hor, ver);

		logger.debug() << "Detected desktop resolution: "
			<< hor << " x " << ver << std::endl;
	}
	Resolution desktopRes = { hor, ver };

	writeResolutionTuple("Res Mode 3 GFX", getGFXAddress(),
		desktopRes, GfxEngine::mode3X, GfxEngine::mode3Y);
	writeResolutionTuple("Res Mode 3 EXE", getBaseAddress(),
		desktopRes, S4::mode3X, S4::mode3Y);

	logger.info("creating GUI hook...");
	createGuiHook(cf->customResolution2, desktopRes, cf->bCustomRes3);

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

int param1;
int param2;
char* param3;
DWORD jmpBackAddr;
void __declspec(naked) setText(){
	__asm {
		push eax

		mov eax, [esp+0x8+0x4]
		mov [param1], eax

		mov eax, [esp+0xC+0x4]
		mov [param2], eax

		mov eax, [esp+0x4+0x4]
		mov [param3], eax

		pop eax
		pushad
	}
	std::cout << param1 << " "
		<< param2 << " "
		<< param3 << " " << *param3 << " "
		<< *(int*)param3 << " "
		<< *(short*)param3 << " "
		<< std::endl;
	__asm {
		popad

		push ebx
		xor ebx, ebx
		push ebp
		push esi

		jmp [jmpBackAddr]
	}
}

Resolution resMode2;
Resolution resMode3;
bool resMode3custom;
const char defMode2[] = "1024 x 768";
const char defMode3[] = "1280 x 1024";
int strLength;
char* string;
char newString[20];
DWORD jmpBackAddr2;
void _textUpdated() {
	if (strLength == 10 || strLength == 11) {
		if (strncmp(defMode2, string, strLength) == 0) {
			std::stringstream ss;
			ss << resMode2.x << " x " << resMode2.y << " custom";
			auto ns = ss.str();

			guiLogger.debug() << string << " (" << strLength << ") -> ";

			strLength = ns.length();
			strncpy_s(newString, ns.c_str(), strLength);
			string = newString;

			guiLogger.naked() << string << " (" << strLength << ")" << std::endl;
		}
		else if (strncmp(defMode3, string, strLength) == 0) {
			std::stringstream ss;
			ss << resMode3.x << " x " << resMode3.y;
			if (resMode3custom)
				ss << " custom";
			else
				ss << " native";
			auto ns = ss.str();

			guiLogger.debug() << string << " (" << strLength << ") -> ";

			strLength = ns.length();
			strncpy_s(newString, ns.c_str(), strLength);
			string = newString;

			guiLogger.naked() << string << " (" << strLength << ")" << std::endl;
		}
	}
}
void __declspec(naked) textUpdated() {
	__asm {
		mov [strLength], edi
		mov [string], ebp

		pushad
	}
	_textUpdated();
	__asm {
		popad 

		mov edi, [strLength]
		mov ebp, [string]

		push edi
		push ebp
		lea eax, [eax+eax*2]

		jmp [jmpBackAddr2]
	}
}

void createGuiHook(Resolution mode2, Resolution mode3, bool mode3custom) {
	resMode2 = mode2;
	resMode3 = mode3;
	resMode3custom = mode3custom;

	//DWORD addr = (DWORD)getGuiAddress() + GuiEngine_SetText + 6;
	//functionInjectorReturn((DWORD*)addr, setText, jmpBackAddr, 5);

	DWORD addr = (DWORD)getGuiAddress() + GuiEngine_SetText + 0x1CC;
	functionInjectorReturn((DWORD*)addr, textUpdated, jmpBackAddr2, 5);
}
