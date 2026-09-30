#pragma once
#include <Arduino.h>
#include "AntBoy_Pins.h"

enum AntButton {
    ANT_BTN_A = 0,
    ANT_BTN_B,
    ANT_BTN_SELECT,
    ANT_BTN_START,
    ANT_BTN_MENU,
    ANT_BTN_VOL,
    ANT_BTN_UP,
    ANT_BTN_DOWN,
    ANT_BTN_LEFT,
    ANT_BTN_RIGHT,
    ANT_BTN_COUNT
};

enum AntDirection {
    ANT_DIR_NONE = 0,
    ANT_DIR_UP,
    ANT_DIR_DOWN,
    ANT_DIR_LEFT,
    ANT_DIR_RIGHT
};

class AntBoy_ButtonsClass {
public:
    void begin();
    void update();

    // Query status tombol
    bool isPressed(AntButton btn) const;
    bool wasPressed(AntButton btn) const;
    bool wasReleased(AntButton btn) const;
    bool anyPressed() const { return _currentState != 0; }
    bool anyWasPressed() const { return _pressedEvents != 0; }

    // Helper arah D-Pad
    AntDirection readDpad() const;

    // Nilai analog terfilter untuk kalibrasi / diagnostik
    int getADC_Vertical() const { return _lastAdcVert; }
    int getADC_Horizontal() const { return _lastAdcHorz; }

    // Deteksi kombinasi shortcut tahan (misal SELECT + START selama N ms)
    bool isHoldingCombo(AntButton btn1, AntButton btn2, uint32_t holdTimeMs) const;

private:
    uint16_t _currentState = 0;
    uint16_t _previousState = 0;
    uint16_t _pressedEvents = 0;
    uint16_t _releasedEvents = 0;

    int _lastAdcVert = 0;
    int _lastAdcHorz = 0;

    // Timing Debounce untuk 6 tombol digital
    uint32_t _lastDebounceTime[6] = {0};
    bool _debouncedDigital[6] = {false};
    bool _lastReadingDigital[6] = {false};

    // Filter median ADC 3-sampel
    int readFilteredADC(int pin);

    // Tracking combo holding
    mutable uint32_t _comboStartTime = 0;
};
