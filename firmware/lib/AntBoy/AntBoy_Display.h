#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "AntBoy_Pins.h"

#define ANTBOY_SCREEN_WIDTH   320
#define ANTBOY_SCREEN_HEIGHT  240

// Definisi Warna RGB565 Populer
#define ANTBOY_COLOR_BLACK       0x0000
#define ANTBOY_COLOR_WHITE       0xFFFF
#define ANTBOY_COLOR_RED         0xF800
#define ANTBOY_COLOR_GREEN       0x07E0
#define ANTBOY_COLOR_BLUE        0x001F
#define ANTBOY_COLOR_CYAN        0x07FF
#define ANTBOY_COLOR_MAGENTA     0xF81F
#define ANTBOY_COLOR_YELLOW      0xFFE0
#define ANTBOY_COLOR_ORANGE      0xFD20
#define ANTBOY_COLOR_PURPLE      0x780F
#define ANTBOY_COLOR_DARKGREY    0x39E7
#define ANTBOY_COLOR_LIGHTGREY   0xC618
#define ANTBOY_COLOR_NAVY        0x000F
#define ANTBOY_COLOR_DARKGREEN   0x03E0

// Palet Retro Khas Game Boy Klasik 4-Shade
#define ANTBOY_PALETTE_GB_WHITE  0x9EE7
#define ANTBOY_PALETTE_GB_LIGHT  0x8E66
#define ANTBOY_PALETTE_GB_DARK   0x3444
#define ANTBOY_PALETTE_GB_BLACK  0x1261

class AntBoy_DisplayClass : public Adafruit_ST7789 {
public:
    AntBoy_DisplayClass();

    void begin();

    // Kontrol Lampu Latar (Backlight PWM pada IO14)
    void setBrightness(uint8_t percent); // 0 - 100%
    uint8_t getBrightness() const { return _brightnessPercent; }

    // Helper UI Khusus
    void drawCenteredText(const char* text, int y, uint16_t color, uint8_t size = 1);
    void drawHeaderBar(const char* title, uint16_t bgColor = ANTBOY_COLOR_NAVY);
    void drawFooterBar(const char* info, uint16_t bgColor = ANTBOY_COLOR_DARKGREY);

private:
    uint8_t _brightnessPercent = 100;
};
