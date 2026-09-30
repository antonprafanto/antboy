#include <Arduino.h>
#include <AntBoy.h>

void setup() {
    Serial.begin(115200);

    // Inisialisasi seluruh periferal ANTBOY
    AntBoy.begin(true);

    // Mainkan startup jingle bawaan
    AntBoy.Audio.playStartupJingle();

    // Gambar tampilan awal
    AntBoy.Display.fillScreen(ANTBOY_COLOR_BLACK);
    AntBoy.Display.drawHeaderBar("ANTBOY (2026) — Hello World", ANTBOY_COLOR_NAVY);
    AntBoy.Display.drawCenteredText("HELLO ANTBOY!", 100, ANTBOY_COLOR_GREEN, 2);
    AntBoy.Display.drawFooterBar("Tekan tombol A / B / D-Pad", ANTBOY_COLOR_DARKGREY);
}

void loop() {
    // Wajib dipanggil di awal setiap iterasi loop
    AntBoy.update();

    // Respons terhadap tombol aksi A & B
    if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
        AntBoy.Audio.playClick();
        AntBoy.toggleLED();
        AntBoy.Display.fillRect(40, 130, 240, 20, ANTBOY_COLOR_BLACK);
        AntBoy.Display.drawCenteredText("Button A Pressed!", 130, ANTBOY_COLOR_YELLOW, 1);
    }

    if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
        AntBoy.Audio.playConfirm();
        AntBoy.Display.fillRect(40, 130, 240, 20, ANTBOY_COLOR_BLACK);
        AntBoy.Display.drawCenteredText("Button B Pressed!", 130, ANTBOY_COLOR_CYAN, 1);
    }

    // Deteksi D-Pad
    AntDirection dir = AntBoy.Buttons.readDpad();
    if (dir != ANT_DIR_NONE) {
        AntBoy.Display.fillRect(40, 155, 240, 20, ANTBOY_COLOR_BLACK);
        if (dir == ANT_DIR_UP)    AntBoy.Display.drawCenteredText("D-Pad: UP", 155, ANTBOY_COLOR_WHITE, 1);
        if (dir == ANT_DIR_DOWN)  AntBoy.Display.drawCenteredText("D-Pad: DOWN", 155, ANTBOY_COLOR_WHITE, 1);
        if (dir == ANT_DIR_LEFT)  AntBoy.Display.drawCenteredText("D-Pad: LEFT", 155, ANTBOY_COLOR_WHITE, 1);
        if (dir == ANT_DIR_RIGHT) AntBoy.Display.drawCenteredText("D-Pad: RIGHT", 155, ANTBOY_COLOR_WHITE, 1);
    }

    // Kontrol Volume Cepat
    if (AntBoy.Buttons.wasPressed(ANT_BTN_VOL)) {
        AntBoy.Audio.cycleVolume();
    }

    delay(16); // Target ~60 FPS
}
