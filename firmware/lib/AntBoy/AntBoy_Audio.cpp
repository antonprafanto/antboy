#include "AntBoy_Audio.h"

void AntBoy_AudioClass::begin() {
    pinMode(ANTBOY_PIN_BUZZER, OUTPUT);
    digitalWrite(ANTBOY_PIN_BUZZER, LOW);

    // Inisialisasi Hardware LEDC Timer ESP32 untuk Kontrol Nada & Volume Duty Cycle
    ledcSetup(_ledcChannel, 2000, _ledcResolution);
    ledcAttachPin(ANTBOY_PIN_BUZZER, _ledcChannel);
    stopTone();
}

uint8_t AntBoy_AudioClass::getDutyCycleForVolume() const {
    switch (_volumeLevel) {
        case ANT_VOL_MUTE: return 0;
        case ANT_VOL_LOW:  return 8;   // ~3% duty cycle (suara lirih / hening)
        case ANT_VOL_MED:  return 25;  // ~10% duty cycle (suara sedang)
        case ANT_VOL_HIGH: return 64;  // ~25% duty cycle (suara jelas)
        case ANT_VOL_MAX:  return 128; // 50% square wave (amplitudo akustik maksimum)
        default:           return 64;
    }
}

void AntBoy_AudioClass::playTone(uint16_t freq, uint16_t durationMs) {
    if (_volumeLevel == ANT_VOL_MUTE || freq == 0) {
        stopTone();
        return;
    }

    // Set frekuensi nada
    ledcWriteTone(_ledcChannel, freq);

    // Modulasi lebar pulsa (Duty Cycle) untuk menghasilkan volume yang benar-benar berbeda
    ledcWrite(_ledcChannel, getDutyCycleForVolume());

    if (durationMs > 0) {
        delay(durationMs);
        stopTone();
    }
}

void AntBoy_AudioClass::stopTone() {
    ledcWrite(_ledcChannel, 0);
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
    playTone(1760, 70); delay(15);
    playTone(2093, 70); delay(15);
    playTone(2637, 70); delay(15);
    playTone(3520, 160);
}

void AntBoy_AudioClass::playClick() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(2637, 25);
}

void AntBoy_AudioClass::playConfirm() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(2093, 40); delay(20);
    playTone(3136, 90);
}

void AntBoy_AudioClass::playWarning() {
    if (_volumeLevel == ANT_VOL_MUTE) return;
    playTone(1046, 90); delay(20);
    playTone(880, 140);
}
