#pragma once

#include "Config.h"

const unsigned int ver_maj = 1;
const unsigned int ver_min = 1;

void showHeaderMessage();

void patchResolutions(ConfigData* cf);
void patchVideoSkip(char* path, ConfigData* cf);

void createGuiHook(Resolution mode2, Resolution mode3, bool mode3custom);
