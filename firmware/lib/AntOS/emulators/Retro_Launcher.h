#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"
#include "Chip8_Runner.h"

enum RetroConsoleType {
    RETRO_TYPE_NES = 0,
    RETRO_TYPE_SMS,
    RETRO_TYPE_ATARI
};

class Retro_LauncherClass {
public:
    void run(RetroConsoleType consoleType);

private:
    RetroConsoleType currentConsole = RETRO_TYPE_NES;
    char romFileList[16][64];
    uint32_t romFileSizes[16];
    int romCount = 0;
    int selectedRomIndex = 0;

    void scanRoms();
    void renderBrowser();
    void showRomInfo(const char* filename);
};

extern Retro_LauncherClass Retro_Launcher;
