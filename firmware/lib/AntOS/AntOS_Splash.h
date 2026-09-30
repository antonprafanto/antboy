#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"

class AntOS_SplashClass {
public:
    // Menjalankan sekuens bootloader & animasi splash screen
    void run();

private:
    void drawBootLogo();
    void updateProgress(uint8_t percent, const char* statusMsg);
};

extern AntOS_SplashClass AntOS_Splash;
