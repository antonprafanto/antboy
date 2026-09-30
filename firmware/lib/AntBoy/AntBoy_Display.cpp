#include "AntBoy_Display.h"

AntBoy_DisplayClass::AntBoy_DisplayClass() 
    : Adafruit_ST7789(ANTBOY_PIN_TFT_CS, ANTBOY_PIN_TFT_DC, ANTBOY_PIN_TFT_RST) {
}

void AntBoy_DisplayClass::begin() {
    // 1. Inisialisasi Backlight (IO14)
    pinMode(ANTBOY_PIN_TFT_BLK, OUTPUT);
    digitalWrite(ANTBOY_PIN_TFT_BLK, HIGH); // Default ON

    // 2. Inisialisasi Layar ST7789
    init(240, 320, SPI_MODE3);
    setRotation(3);           // Landscape Mode 320x240
    invertDisplay(true);      // Koreksi Inversi Warna Panel IPS GMT020-03-SD
    fillScreen(ANTBOY_COLOR_BLACK);

    // 3. Set Kecerahan Awal 100%
    setBrightness(100);
}

void AntBoy_DisplayClass::setBrightness(uint8_t percent) {
    if (percent > 100) percent = 100;
    _brightnessPercent = percent;

    // Gunakan analogWrite (otomatis memetakan ke hardware LEDC timer ESP32)
    uint8_t duty = (uint8_t)((uint32_t)percent * 255 / 100);
    analogWrite(ANTBOY_PIN_TFT_BLK, duty);
}

void AntBoy_DisplayClass::drawCenteredText(const char* text, int y, uint16_t color, uint8_t size) {
    setTextSize(size);
    setTextColor(color);
    
    // Estimasi lebar karakter bawaan Adafruit_GFX (6 pixel per karakter pada size 1)
    int charWidth = 6 * size;
    int strLen = strlen(text);
    int totalWidth = strLen * charWidth;
    int x = (ANTBOY_SCREEN_WIDTH - totalWidth) / 2;
    if (x < 0) x = 0;

    setCursor(x, y);
    print(text);
}

void AntBoy_DisplayClass::drawHeaderBar(const char* title, uint16_t bgColor) {
    fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 22, bgColor);
    drawFastHLine(0, 22, ANTBOY_SCREEN_WIDTH, ANTBOY_COLOR_CYAN);
    
    setTextSize(1);
    setTextColor(ANTBOY_COLOR_WHITE, bgColor);
    setCursor(10, 7);
    print(title);
}

void AntBoy_DisplayClass::drawFooterBar(const char* info, uint16_t bgColor) {
    fillRect(0, ANTBOY_SCREEN_HEIGHT - 18, ANTBOY_SCREEN_WIDTH, 18, bgColor);
    drawFastHLine(0, ANTBOY_SCREEN_HEIGHT - 19, ANTBOY_SCREEN_WIDTH, ANTBOY_COLOR_DARKGREY);
    
    setTextSize(1);
    setTextColor(ANTBOY_COLOR_LIGHTGREY, bgColor);
    setCursor(10, ANTBOY_SCREEN_HEIGHT - 13);
    print(info);
}
