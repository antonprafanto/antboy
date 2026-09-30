#include "AntOS.h"

AntOSClass AntOS;

void AntOSClass::begin() {
    // 1. Inisialisasi Hardware HAL AntBoy (Layar, Tombol, Buzzer, SD, Power)
    AntBoy.begin(true);

    // 2. Muat preferensi pengguna dari Flash NVS (Volume & Kecerahan Layar)
    Settings.begin();

    // 3. Jalankan Bootloader Sequence & Animasi Splash Screen
    Splash.run();

    // 4. Inisialisasi subsistem UI AntOS
    StatusBar.begin();
    Launcher.begin();
    QuickSettings.begin();

    // 5. Render tampilan awal
    requestRedraw();
}

void AntOSClass::requestRedraw() {
    _redrawPending = true;
}

void AntOSClass::update() {
    // Polling status tombol fisik
    AntBoy.update();

    // 1. Tangani Global Exit Shortcut (Tahan SELECT + START selama 2 detik)
    if (AntBoy.checkExitShortcut()) {
        if (QuickSettings.isOpen()) {
            QuickSettings.close();
        } else if (Launcher.isInSubMenu()) {
            // Kembali ke menu utama carousel
            Launcher.begin();
        }
        AntBoy.Audio.playWarning();
        requestRedraw();
        delay(400);
        return;
    }

    // 2. Tangani Tombol VOL (Siklus Volume Cepat + OSD Toast)
    if (AntBoy.Buttons.wasPressed(ANT_BTN_VOL)) {
        uint8_t newVol = Settings.cycleVolume();
        StatusBar.triggerVolumeOSD(AntBoy.Audio.getVolumeString(), newVol);
        requestRedraw();
    }

    // 3. Tangani Tombol MENU jika Quick Settings belum aktif
    if (!QuickSettings.isOpen() && AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
        QuickSettings.open();
        requestRedraw();
        return;
    }

    // 4. Delegasi Input Berdasarkan State UI
    if (QuickSettings.isOpen()) {
        bool wasOpen = true;
        QuickSettings.handleInput();
        if (!QuickSettings.isOpen()) {
            // Baru saja ditutup, gambar ulang launcher
            requestRedraw();
        }
    } else {
        if (Launcher.handleInput()) {
            requestRedraw();
        }
    }

    // 5. Update Timer & Animasi
    StatusBar.update();
    Launcher.update();

    // 6. Rendering Frame
    StatusBar.render(_redrawPending);
    if (QuickSettings.isOpen()) {
        QuickSettings.render();
    } else {
        Launcher.render(_redrawPending);
    }

    _redrawPending = false;
}
