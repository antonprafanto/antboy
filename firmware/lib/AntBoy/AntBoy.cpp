#include "AntBoy.h"

AntBoyClass AntBoy;

void AntBoyClass::begin(bool initSD) {
    // 1. Inisialisasi Power & Status LED
    Power.begin();
    setLED(true); // Nyalakan LED saat boot

    // 2. Inisialisasi Audio Buzzer
    Audio.begin();

    // 3. Inisialisasi Input Tombol & D-Pad ADC
    Buttons.begin();

    // 4. Inisialisasi Layar ST7789 Landscape 320x240
    Display.begin();

    // 5. Inisialisasi MicroSD jika diminta
    if (initSD) {
        SD.begin();
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
