#include "AntBoy_Buttons.h"

void AntBoy_ButtonsClass::begin() {
    // Inisialisasi tombol digital aktif LOW
    pinMode(ANTBOY_PIN_BTN_A, INPUT_PULLUP);
    pinMode(ANTBOY_PIN_BTN_B, INPUT_PULLUP);
    pinMode(ANTBOY_PIN_BTN_SELECT, INPUT_PULLUP);
    pinMode(ANTBOY_PIN_BTN_START, INPUT);          // GPIO 39 input-only, pullup eksternal R9
    pinMode(ANTBOY_PIN_BTN_MENU, INPUT_PULLUP);
    pinMode(ANTBOY_PIN_BTN_VOL, INPUT_PULLUP);

    // Inisialisasi ADC D-Pad
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    _currentState = 0;
    _previousState = 0;
    _pressedEvents = 0;
    _releasedEvents = 0;

    for (int i = 0; i < 6; i++) {
        _lastDebounceTime[i] = 0;
        _debouncedDigital[i] = false;
        _lastReadingDigital[i] = false;
    }
}

int AntBoy_ButtonsClass::readFilteredADC(int pin) {
    // Median Filter 3-Sampel untuk meredam noise switching dan ripple resistor ladder
    int a = analogRead(pin);
    delayMicroseconds(50);
    int b = analogRead(pin);
    delayMicroseconds(50);
    int c = analogRead(pin);

    // Median of 3 values
    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

void AntBoy_ButtonsClass::update() {
    _previousState = _currentState;
    uint16_t state = 0;
    uint32_t now = millis();

    // 1. Baca & Debounce 6 Tombol Digital (Active-LOW: 0 = Ditekan)
    const uint8_t digitalPins[6] = {
        ANTBOY_PIN_BTN_A,
        ANTBOY_PIN_BTN_B,
        ANTBOY_PIN_BTN_SELECT,
        ANTBOY_PIN_BTN_START,
        ANTBOY_PIN_BTN_MENU,
        ANTBOY_PIN_BTN_VOL
    };

    for (int i = 0; i < 6; i++) {
        bool rawPressed = (digitalRead(digitalPins[i]) == LOW);
        if (rawPressed != _lastReadingDigital[i]) {
            _lastDebounceTime[i] = now;
            _lastReadingDigital[i] = rawPressed;
        }

        // Terapkan batas waktu debounce 18 ms
        if ((now - _lastDebounceTime[i]) >= 18) {
            _debouncedDigital[i] = rawPressed;
        }

        if (_debouncedDigital[i]) {
            state |= (1 << i);
        }
    }

    // 2. Baca D-Pad Vertikal (IO35) dengan Median Filter & Threshold Histeresis
    _lastAdcVert = readFilteredADC(ANTBOY_PIN_DPAD_VERT);
    if (_lastAdcVert > 3100) {
        state |= (1 << ANT_BTN_UP);
    } else if (_lastAdcVert > 1200 && _lastAdcVert < 2900) {
        state |= (1 << ANT_BTN_DOWN);
    }

    // 3. Baca D-Pad Horisontal (IO34) dengan Median Filter & Threshold Histeresis
    _lastAdcHorz = readFilteredADC(ANTBOY_PIN_DPAD_HORZ);
    if (_lastAdcHorz > 3100) {
        state |= (1 << ANT_BTN_LEFT);
    } else if (_lastAdcHorz > 1200 && _lastAdcHorz < 2900) {
        state |= (1 << ANT_BTN_RIGHT);
    }

    _currentState = state;

    // Event edge detection
    _pressedEvents = (_currentState ^ _previousState) & _currentState;
    _releasedEvents = (_currentState ^ _previousState) & _previousState;
}

bool AntBoy_ButtonsClass::isPressed(AntButton btn) const {
    if (btn >= ANT_BTN_COUNT) return false;
    return (_currentState & (1 << btn)) != 0;
}

bool AntBoy_ButtonsClass::wasPressed(AntButton btn) const {
    if (btn >= ANT_BTN_COUNT) return false;
    return (_pressedEvents & (1 << btn)) != 0;
}

bool AntBoy_ButtonsClass::wasReleased(AntButton btn) const {
    if (btn >= ANT_BTN_COUNT) return false;
    return (_releasedEvents & (1 << btn)) != 0;
}

AntDirection AntBoy_ButtonsClass::readDpad() const {
    if (isPressed(ANT_BTN_UP))    return ANT_DIR_UP;
    if (isPressed(ANT_BTN_DOWN))  return ANT_DIR_DOWN;
    if (isPressed(ANT_BTN_LEFT))  return ANT_DIR_LEFT;
    if (isPressed(ANT_BTN_RIGHT)) return ANT_DIR_RIGHT;
    return ANT_DIR_NONE;
}

bool AntBoy_ButtonsClass::isHoldingCombo(AntButton btn1, AntButton btn2, uint32_t holdTimeMs) const {
    if (isPressed(btn1) && isPressed(btn2)) {
        static uint32_t comboStart = 0;
        if (comboStart == 0) {
            comboStart = millis();
        }
        if (millis() - comboStart >= holdTimeMs) {
            return true;
        }
    } else {
        // Reset jika salah satu dilepas
        static uint32_t comboStart = 0;
        comboStart = 0;
    }
    return false;
}
