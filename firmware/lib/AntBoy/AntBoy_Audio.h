#pragma once
#include <Arduino.h>
#include "AntBoy_Pins.h"

enum AntVolumeLevel {
    ANT_VOL_MUTE = 0,
    ANT_VOL_LOW,
    ANT_VOL_MED,
    ANT_VOL_HIGH,
    ANT_VOL_MAX,
    ANT_VOL_LEVEL_COUNT
};

class AntBoy_AudioClass {
public:
    void begin();

    // Memainkan nada frekuensi (Hz) dengan durasi (ms)
    void playTone(uint16_t freq, uint16_t durationMs = 0);
    void stopTone();

    // Kontrol Volume (Modulasi True PWM Duty Cycle 0% s.d. 50%)
    void setVolume(AntVolumeLevel level);
    AntVolumeLevel cycleVolume(); // Siklus Mute -> Low -> Med -> High -> Max -> Mute
    AntVolumeLevel getVolume() const { return _volumeLevel; }
    const char* getVolumeString() const;
    uint8_t getDutyCycleForVolume() const;

    // Jingle & Sound Effects Bawaan (Menggunakan Titik Resonansi 1.7 - 3.1 kHz)
    void playStartupJingle();
    void playClick();
    void playConfirm();
    void playWarning();

private:
    AntVolumeLevel _volumeLevel = ANT_VOL_HIGH;
    uint8_t _ledcChannel = 2;
    uint8_t _ledcResolution = 8;
};
