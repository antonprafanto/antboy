#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"
#ifdef __cplusplus
extern "C" {
#endif
#define PEANUT_GB_HEADER_ONLY
#include "peanut_gb.h"
#ifdef __cplusplus
}
#endif

class PeanutGB_RunnerClass {
public:
    void run();

private:
    struct gb_s gb;
    uint8_t* fullRom = nullptr;
    uint32_t fullRomSize = 0;
    uint8_t cartRam[8192];
    File romFile;
    bool hasRomLoaded = false;
    uint8_t currentPalette = 0;

    // ROM Browser
    char romFileList[16][64];
    uint32_t romFileSizes[16];
    int romCount = 0;
    int selectedRomIndex = 0;

    void scanRoms();
    void renderBrowser();
    bool loadRom(const char* filename);
    void closeRom();
    void runEmulationLoop();

    static uint8_t romReadCallback(struct gb_s *gb, const uint_fast32_t addr);
    static uint8_t ramReadCallback(struct gb_s *gb, const uint_fast32_t addr);
    static void ramWriteCallback(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val);
    static void errorCallback(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr);
    static void drawLineCallback(struct gb_s *gb, const uint8_t *pixels, const uint_fast8_t line);
};

extern PeanutGB_RunnerClass PeanutGB_Runner;
