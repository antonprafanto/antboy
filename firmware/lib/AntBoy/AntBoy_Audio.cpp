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

void AntBoy_AudioClass::playRTTTL(const char* p) {
    if (_volumeLevel == ANT_VOL_MUTE || p == nullptr) return;

    const uint16_t notes[] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };

    // 1. Lewati judul lagu hingga titik dua pertama ':'
    while (*p && *p != ':') p++;
    if (!*p) return;
    p++;

    // 2. Parse default settings (d=4,o=5,b=140)
    int default_dur = 4;
    int default_oct = 5;
    int bpm = 120;

    while (*p && *p != ':') {
        if (*p == 'd') {
            p += 2;
            default_dur = 0;
            while (isdigit(*p)) default_dur = default_dur * 10 + (*p++ - '0');
        } else if (*p == 'o') {
            p += 2;
            default_oct = *p++ - '0';
        } else if (*p == 'b') {
            p += 2;
            bpm = 0;
            while (isdigit(*p)) bpm = bpm * 10 + (*p++ - '0');
        } else {
            p++;
        }
    }
    if (!*p) return;
    p++;

    if (bpm <= 0) bpm = 120;
    long wholenote = (60000L * 4) / bpm;

    // 3. Mainkan rangkaian nada
    while (*p) {
        while (*p == ',' || *p == ' ') p++;
        if (!*p) break;

        int dur = 0;
        while (isdigit(*p)) dur = dur * 10 + (*p++ - '0');
        if (dur == 0) dur = default_dur;

        char note = *p++;
        int note_idx = -1;
        bool is_pause = false;

        switch (note) {
            case 'c': note_idx = 0; break;
            case 'd': note_idx = 2; break;
            case 'e': note_idx = 4; break;
            case 'f': note_idx = 5; break;
            case 'g': note_idx = 7; break;
            case 'a': note_idx = 9; break;
            case 'b': note_idx = 11; break;
            case 'p': is_pause = true; break;
            default: break;
        }

        if (*p == '#') {
            p++;
            if (note_idx >= 0) note_idx++;
        }

        bool dotted = false;
        if (*p == '.') {
            dotted = true;
            p++;
        }

        int oct = default_oct;
        if (isdigit(*p)) oct = *p++ - '0';

        if (*p == '.') {
            dotted = true;
            p++;
        }

        long duration = wholenote / dur;
        if (dotted) duration += (duration / 2);

        if (is_pause || note_idx < 0) {
            stopTone();
            delay(duration);
        } else {
            int shift = oct - 4;
            uint16_t freq = (shift >= 0) ? (notes[note_idx] << shift) : (notes[note_idx] >> (-shift));
            playTone(freq, (uint16_t)(duration * 0.85));
            delay((uint16_t)(duration * 0.15));
        }
    }
    stopTone();
}
