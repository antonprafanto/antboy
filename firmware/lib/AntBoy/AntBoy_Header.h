#pragma once
#include <Arduino.h>
#include "AntBoy_Pins.h"

// =========================================================================
// ANTBOY (2026) SIDE EXPANSION HEADER (J4) SAFE ACCESSOR CLASS
// =========================================================================

class AntBoy_HeaderClass {
public:
    void begin() {
        // Konfigurasi awal pin ekspansi bebas ke mode aman (INPUT_PULLUP)
        ::pinMode(ANTBOY_PIN_EXP_IO4, INPUT_PULLUP);
        ::pinMode(ANTBOY_PIN_EXP_IO16, INPUT_PULLUP);
        ::pinMode(ANTBOY_PIN_EXP_IO25, INPUT_PULLUP);
    }

    // Hanya memperbolehkan akses ke pin yang 100% bebas dari bus internal
    bool isSafePin(uint8_t pin) const {
        return (pin == ANTBOY_PIN_EXP_IO4 || pin == ANTBOY_PIN_EXP_IO16 || pin == ANTBOY_PIN_EXP_IO25);
    }

    void pinMode(uint8_t pin, uint8_t mode) {
        if (isSafePin(pin)) {
            ::pinMode(pin, mode);
        }
    }

    int digitalRead(uint8_t pin) const {
        if (isSafePin(pin)) {
            return ::digitalRead(pin);
        }
        return LOW;
    }

    void digitalWrite(uint8_t pin, uint8_t val) {
        if (isSafePin(pin)) {
            ::digitalWrite(pin, val);
        }
    }

    int analogRead(uint8_t pin) {
        if (pin == ANTBOY_PIN_EXP_IO4) {
            return ::analogRead(pin);
        }
        return 0;
    }

    void dacWrite(uint8_t val) {
        // True 8-bit DAC1 pada IO25 (PRD Pilar 4 / J4 Pin 11)
        ::dacWrite(ANTBOY_PIN_EXP_IO25, val);
    }
};
