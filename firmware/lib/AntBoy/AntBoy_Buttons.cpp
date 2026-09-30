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
}

void AntBoy_ButtonsClass::update() {
    _previousState = _currentState;
    uint16_t state = 0;

    // 1. Baca Tombol Digital (Active-LOW: 0 = Ditekan)
    if (digitalRead(ANTBOY_PIN_BTN_A) == LOW)      state |= (1 << ANT_BTN_A);
    if (digitalRead(ANTBOY_PIN_BTN_B) == LOW)      state |= (1 << ANT_BTN_B);
    if (digitalRead(ANTBOY_PIN_BTN_SELECT) == LOW) state |= (1 << ANT_BTN_SELECT);
    if (digitalRead(ANTBOY_PIN_BTN_START) == LOW)  state |= (1 << ANT_BTN_START);
    if (digitalRead(ANTBOY_PIN_BTN_MENU) == LOW)   state |= (1 << ANT_BTN_MENU);
    if (digitalRead(ANTBOY_PIN_BTN_VOL) == LOW)    state |= (1 << ANT_BTN_VOL);

    // 2. Baca D-Pad Vertikal (IO35)
    _lastAdcVert = analogRead(ANTBOY_PIN_DPAD_VERT);
    if (_lastAdcVert > 3000) {
        state |= (1 << ANT_BTN_UP);
    } else if (_lastAdcVert > 1000) {
        state |= (1 << ANT_BTN_DOWN);
    }

    // 3. Baca D-Pad Horisontal (IO34)
    _lastAdcHorz = analogRead(ANTBOY_PIN_DPAD_HORZ);
    if (_lastAdcHorz > 3000) {
        state |= (1 << ANT_BTN_LEFT);
    } else if (_lastAdcHorz > 1000) {
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
