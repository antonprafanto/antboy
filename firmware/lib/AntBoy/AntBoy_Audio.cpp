#include "AntBoy_Audio.h"

void AntBoy_AudioClass::begin() {
    pinMode(ANTBOY_PIN_BUZZER, OUTPUT);
    digitalWrite(ANTBOY_PIN_BUZZER, LOW);
}

void AntBoy_AudioClass::playTone(uint16_t freq, uint16_t durationMs) {
    if (_volumeLevel == ANT_VOL_MUTE || freq == 0) {
        stopTone();
        return;
    }

    // Mainkan nada via fungsi tone bawaan ESP32 Arduino
    if (durationMs > 0) {
        tone(ANTBOY_PIN_BUZZER, freq, durationMs);
    } else {
        tone(ANTBOY_PIN_BUZZER, freq);
    }
}

void AntBoy_AudioClass::stopTone() {
    noTone(ANTBOY_PIN_BUZZER);
    digitalWrite(ANTBOY_PIN_BUZZER, LOW);
}

void AntBoy_AudioClass::setVolume(AntVolumeLevel level) {
    if (level >= ANT_VOL_LEVEL_COUNT) level = ANT_VOL_MAX;
    _volumeLevel = level;
    if (_volumeLevel == ANT_VOL_MUTE) {
        stopTone();
    }
}

AntVolumeLevel AntBoy_AudioClass::cycleVolume() {
    int next = (int)_volumeLevel + 1;
    if (next >= ANT_VOL_LEVEL_COUNT) next = 0;
    setVolume((AntVolumeLevel)next);
    
    // Nada feedback saat volume berubah
    if (_volumeLevel != ANT_VOL_MUTE) {
        playTone(2093 + (_volumeLevel * 200), 50);
    }
    return _volumeLevel;
}

const char* AntBoy_AudioClass::getVolumeString() const {
    switch (_volumeLevel) {
        case ANT_VOL_MUTE: return "MUTE";
        case ANT_VOL_LOW:  return "25%";
        case ANT_VOL_MED:  return "50%";
        case ANT_VOL_HIGH: return "75%";
        case ANT_VOL_MAX:  return "100%";
        default:           return "UNKNOWN";
    }
}

void AntBoy_AudioClass::playStartupJingle() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(1760, 70); delay(85);
    playTone(2093, 70); delay(85);
    playTone(2637, 70); delay(85);
    playTone(3520, 160); delay(180);
}

void AntBoy_AudioClass::playClick() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(2637, 25);
}

void AntBoy_AudioClass::playConfirm() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(2093, 40); delay(50);
    playTone(3136, 90);
}

void AntBoy_AudioClass::playWarning() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(1046, 90); delay(110);
    playTone(880, 140);
}
