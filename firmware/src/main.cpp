#include <Arduino.h>
#include <AntBoy.h>
#include <AntOS.h>

void setup() {
    Serial.begin(115200);
    Serial.println("\n=======================================================");
    Serial.println("   ANTBOY (2026) — AntOS Operating System v1.0");
    Serial.println("   Author: Anton Prafanto | Hardware: Wemos D1 Mini32");
    Serial.println("   Mode: Dual-Core 240MHz | Display: ST7789 320x240");
    Serial.println("=======================================================\n");

    // Inisialisasi & jalankan AntOS Bootloader Sequence & UI Launcher
    AntOS.begin();
}

void loop() {
    // Siklus utama AntOS (Input polling, Carousel navigation, Status bar, Quick Settings)
    AntOS.update();

    delay(15); // Loop rate ~60 FPS
}
