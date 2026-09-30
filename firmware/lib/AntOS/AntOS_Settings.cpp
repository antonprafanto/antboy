#include "AntOS_Settings.h"

AntOS_SettingsClass AntOS_Settings;

void AntOS_SettingsClass::begin() {
    load();
    apply();
}

void AntOS_SettingsClass::load() {
    if (_prefs.begin("antos", true)) { // Read-only mode
        brightness = _prefs.getUChar("bright", 80);
        volume = _prefs.getUChar("vol", 3);
        soundEnabled = _prefs.getBool("sound", true);
        autoDimSeconds = _prefs.getUShort("dim", 30);
        _prefs.end();
    }
}

void AntOS_SettingsClass::save() {
    if (_prefs.begin("antos", false)) { // Read-write mode
        _prefs.putUChar("bright", brightness);
        _prefs.putUChar("vol", volume);
        _prefs.putBool("sound", soundEnabled);
        _prefs.putUShort("dim", autoDimSeconds);
        _prefs.end();
    }

    // Jika MicroSD aktif, sinkronkan juga ke /antos/settings.json
    if (AntBoy.SD.isMounted()) {
        char jsonBuf[160];
        snprintf(jsonBuf, sizeof(jsonBuf),
            "{\n  \"brightness\": %d,\n  \"volume\": %d,\n  \"sound\": %s,\n  \"auto_dim\": %d\n}\n",
            brightness, volume, soundEnabled ? "true" : "false", autoDimSeconds);
        AntBoy.SD.writeFile("/antos/settings.json", jsonBuf);
    }
}

void AntOS_SettingsClass::apply() {
    AntBoy.Display.setBrightness(brightness);
    AntBoy.Audio.setVolume((AntVolumeLevel)volume);
}

uint8_t AntOS_SettingsClass::cycleBrightness() {
    brightness += 20;
    if (brightness > 100) brightness = 20;
    apply();
    save();
    return brightness;
}

uint8_t AntOS_SettingsClass::cycleVolume() {
    volume++;
    if (volume >= ANT_VOL_LEVEL_COUNT) volume = 0;
    apply();
    save();
    return volume;
}
