#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

class Chip8_RunnerClass {
public:
    void run();

private:
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint8_t gfx[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t stack[16];
    uint16_t sp;
    uint8_t key[16];

    bool drawFlag;
    bool isRunning;

    void reset();
    void loadBuiltinRom(int gameIdx);
    void emulateCycle();
    void renderDisplay();
    void updateInput();
};

extern Chip8_RunnerClass Chip8_Runner;
