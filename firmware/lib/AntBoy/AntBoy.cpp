#include "AntBoy.h"

AntBoyClass AntBoy;

void AntBoyClass::begin(bool initSD) {
    // 1. Inisialisasi Power & Status LED
    Power.begin();
    setLED(true); // Nyalakan LED saat boot

    // Pastikan kedua pin CS berstatus HIGH (Deselected) sebelum bus SPI aktif
    pinMode(ANTBOY_PIN_SD_CS, OUTPUT);
    digitalWrite(ANTBOY_PIN_SD_CS, HIGH);
    pinMode(ANTBOY_PIN_TFT_CS, OUTPUT);
    digitalWrite(ANTBOY_PIN_TFT_CS, HIGH);

    // 2. Inisialisasi Audio Buzzer dengan hardware LEDC PWM duty cycle
    Audio.begin();

    // 3. Inisialisasi Input Tombol (dengan debouncing & median filter ADC)
    Buttons.begin();

    // 4. Inisialisasi Layar ST7789 Landscape 320x240
    Display.begin();

    // 5. Inisialisasi MicroSD jika diminta
    if (initSD) {
        if (SD.begin()) {
            SD.createStandardDirectories();
            
            // Catat log boot ke kartu SD
            if (SD.lockBus(100)) {
                File logFile = ::SD.open("/antos/boot.log", FILE_APPEND);
                if (logFile) {
                    logFile.println("[AntOS] Boot OK. AntBoy-Core SDK v1.0 initialized.");
                    logFile.close();
                }
                SD.unlockBus();
            }
        }
    }

    setLED(false); // Matikan LED setelah boot selesai
}

void AntBoyClass::update() {
    Buttons.update();
}

void AntBoyClass::setLED(bool state) {
    _ledState = state;
    digitalWrite(ANTBOY_PIN_LED, _ledState ? HIGH : LOW);
}

void AntBoyClass::toggleLED() {
    setLED(!_ledState);
}

bool AntBoyClass::checkExitShortcut() {
    return Buttons.isHoldingCombo(ANT_BTN_SELECT, ANT_BTN_START, 2000);
}
