#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"
#include "peanut_gb.h"

class PeanutGB_RunnerClass {
public:
    void run();

private:
    struct gb_s gb;
    uint8_t* romBank0 = nullptr;
    uint8_t* romBankN = nullptr;
    uint8_t curBankN = 1;
    File romFile;
    bool hasRomLoaded = false;
    uint8_t currentPalette = 0;

    // ROM Browser
    char romFileList[16][32];
    uint32_t romFileSizes[16];
    int romCount = 0;
    int selectedRomIndex = 0;

    void scanRoms();
    void renderBrowser();
    bool loadRom(const char* filename);
    void closeRom();
    void runEmulationLoop();

    static uint8_t romReadCallback(struct gb_s *gb, const uint32_t addr);
    static uint8_t ramReadCallback(struct gb_s *gb, const uint32_t addr);
    static void ramWriteCallback(struct gb_s *gb, const uint32_t addr, const uint8_t val);
    static void errorCallback(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr);
    static void drawLineCallback(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line);
};

extern PeanutGB_RunnerClass PeanutGB_Runner;
