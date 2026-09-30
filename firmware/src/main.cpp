#include <Arduino.h>
#include <AntBoy.h>

// Deklarasi fungsi rendering UI
void drawDashboardBase();
void updateButtonStates();
void showVolumeToast(const char* volStr);

void setup() {
    Serial.begin(115200);
    Serial.println("\n=============================================");
    Serial.println("  ANTBOY (2026) — AntBoy-Core SDK Bring-Up");
    Serial.println("  Hardware: Anton Prafanto | Firmware: AntOS");
    Serial.println("=============================================\n");

    // Inisialisasi seluruh subsistem AntBoy (Layar, Tombol, Buzzer, SD, LED)
    AntBoy.begin(true);

    // Mainkan Boot Jingle Frekuensi Resonan
    AntBoy.Audio.playStartupJingle();

    // Gambar Tampilan Awal Dashboard
    drawDashboardBase();
}

void loop() {
    // 1. Polling Input Tombol & D-Pad
    AntBoy.update();

    // 2. Tangani Tombol VOL (Siklus Volume Suara)
    if (AntBoy.Buttons.wasPressed(ANT_BTN_VOL)) {
        AntVolumeLevel newVol = AntBoy.Audio.cycleVolume();
        showVolumeToast(AntBoy.Audio.getVolumeString());
    }

    // 3. Tangani Tombol MENU (Toggle Status LED & Beep)
    if (AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
        AntBoy.toggleLED();
        AntBoy.Audio.playConfirm();
    }

    // 4. Deteksi Global Shortcut (Tahan SELECT + START selama 2 detik)
    if (AntBoy.checkExitShortcut()) {
        AntBoy.Display.fillRect(40, 90, 240, 60, ANTBOY_COLOR_RED);
        AntBoy.Display.drawRect(38, 88, 244, 64, ANTBOY_COLOR_WHITE);
        AntBoy.Display.drawCenteredText("GLOBAL SHORTCUT!", 105, ANTBOY_COLOR_WHITE, 2);
        AntBoy.Display.drawCenteredText("SELECT + START DETECTED", 130, ANTBOY_COLOR_YELLOW, 1);
        AntBoy.Audio.playWarning();
        delay(1500);
        drawDashboardBase();
    }

    // 5. Update Status Visual Seluruh 10 Tombol di Layar
    updateButtonStates();

    delay(15); // Loop rate ~60 Hz
}

// -------------------------------------------------------------------------
// RENDERING GRAFIS DASHBOARD
// -------------------------------------------------------------------------

void drawDashboardBase() {
    AntBoy.Display.fillScreen(ANTBOY_COLOR_BLACK);

    // Header Bar
    AntBoy.Display.drawHeaderBar("ANTBOY (2026) — Core SDK v1.0", ANTBOY_COLOR_NAVY);

    // Info Sub-Header: Status SD Card & Volume
    char statusBuf[64];
    const char* sdStatus = AntBoy.SD.isMounted() ? AntBoy.SD.cardTypeString() : "No Card";
    snprintf(statusBuf, sizeof(statusBuf), "SD: %s | Vol: %s | Core: 240MHz", sdStatus, AntBoy.Audio.getVolumeString());
    
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTBOY_COLOR_CYAN, ANTBOY_COLOR_BLACK);
    AntBoy.Display.setCursor(10, 28);
    AntBoy.Display.print(statusBuf);

    // Garis Pemisah
    AntBoy.Display.drawFastHLine(10, 40, 300, ANTBOY_COLOR_DARKGREY);

    // Kotak Visualizer D-Pad & Tombol
    AntBoy.Display.drawRoundRect(15, 48, 140, 150, 6, ANTBOY_COLOR_DARKGREY);
    AntBoy.Display.drawCenteredText("D-PAD & ARROW", 54, ANTBOY_COLOR_LIGHTGREY, 1);

    AntBoy.Display.drawRoundRect(165, 48, 140, 150, 6, ANTBOY_COLOR_DARKGREY);
    AntBoy.Display.drawCenteredText("ACTION & FUNC", 54, ANTBOY_COLOR_LIGHTGREY, 1);

    // Footer Bar
    AntBoy.Display.drawFooterBar("Tekan tombol untuk uji visual | s.id/antonprafanto", ANTBOY_COLOR_DARKGREY);
}

void updateButtonStates() {
    // -------------------------------------------------------------
    // 1. Visualisasi D-Pad (Sisi Kiri)
    // -------------------------------------------------------------
    // UP (X: 73, Y: 72)
    uint16_t colUp = AntBoy.Buttons.isPressed(ANT_BTN_UP) ? ANTBOY_COLOR_GREEN : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillRoundRect(73, 72, 24, 24, 4, colUp);
    AntBoy.Display.drawCenteredText("^", 79, ANTBOY_COLOR_WHITE, 1);

    // DOWN (X: 73, Y: 128)
    uint16_t colDown = AntBoy.Buttons.isPressed(ANT_BTN_DOWN) ? ANTBOY_COLOR_GREEN : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillRoundRect(73, 128, 24, 24, 4, colDown);
    AntBoy.Display.drawCenteredText("v", 135, ANTBOY_COLOR_WHITE, 1);

    // LEFT (X: 45, Y: 100)
    uint16_t colLeft = AntBoy.Buttons.isPressed(ANT_BTN_LEFT) ? ANTBOY_COLOR_GREEN : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillRoundRect(45, 100, 24, 24, 4, colLeft);
    AntBoy.Display.drawCenteredText("<", 107, ANTBOY_COLOR_WHITE, 1);

    // RIGHT (X: 101, Y: 100)
    uint16_t colRight = AntBoy.Buttons.isPressed(ANT_BTN_RIGHT) ? ANTBOY_COLOR_GREEN : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillRoundRect(101, 100, 24, 24, 4, colRight);
    AntBoy.Display.drawCenteredText(">", 107, ANTBOY_COLOR_WHITE, 1);

    // Tampilkan ADC D-Pad Real-time di bawah D-Pad
    char adcBuf[32];
    snprintf(adcBuf, sizeof(adcBuf), "V:%04d H:%04d", AntBoy.Buttons.getADC_Vertical(), AntBoy.Buttons.getADC_Horizontal());
    AntBoy.Display.fillRect(25, 165, 120, 12, ANTBOY_COLOR_BLACK);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTBOY_COLOR_LIGHTGREY, ANTBOY_COLOR_BLACK);
    AntBoy.Display.setCursor(25, 165);
    AntBoy.Display.print(adcBuf);

    // -------------------------------------------------------------
    // 2. Visualisasi Tombol Fungsi & Action (Sisi Kanan)
    // -------------------------------------------------------------
    // Baris Fungsi: START, SELECT, VOL, MENU
    uint16_t colSta = AntBoy.Buttons.isPressed(ANT_BTN_START)  ? ANTBOY_COLOR_YELLOW : ANTBOY_COLOR_DARKGREY;
    uint16_t colSel = AntBoy.Buttons.isPressed(ANT_BTN_SELECT) ? ANTBOY_COLOR_YELLOW : ANTBOY_COLOR_DARKGREY;
    uint16_t colVol = AntBoy.Buttons.isPressed(ANT_BTN_VOL)    ? ANTBOY_COLOR_CYAN   : ANTBOY_COLOR_DARKGREY;
    uint16_t colMen = AntBoy.Buttons.isPressed(ANT_BTN_MENU)   ? ANTBOY_COLOR_MAGENTA: ANTBOY_COLOR_DARKGREY;

    AntBoy.Display.fillRoundRect(175, 75, 26, 16, 3, colSta);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTBOY_COLOR_BLACK);
    AntBoy.Display.setCursor(178, 79);
    AntBoy.Display.print("STA");

    AntBoy.Display.fillRoundRect(207, 75, 26, 16, 3, colSel);
    AntBoy.Display.setCursor(210, 79);
    AntBoy.Display.print("SEL");

    AntBoy.Display.fillRoundRect(239, 75, 26, 16, 3, colVol);
    AntBoy.Display.setCursor(242, 79);
    AntBoy.Display.print("VOL");

    AntBoy.Display.fillRoundRect(271, 75, 26, 16, 3, colMen);
    AntBoy.Display.setCursor(274, 79);
    AntBoy.Display.print("MEN");

    // Tombol Action: A & B
    // A (Atas Kanan: 260, 115)
    uint16_t colA = AntBoy.Buttons.isPressed(ANT_BTN_A) ? ANTBOY_COLOR_RED : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillCircle(270, 125, 14, colA);
    AntBoy.Display.setTextColor(ANTBOY_COLOR_WHITE);
    AntBoy.Display.setCursor(267, 121);
    AntBoy.Display.print("A");

    // B (Bawah Kiri dari A: 220, 145)
    uint16_t colB = AntBoy.Buttons.isPressed(ANT_BTN_B) ? ANTBOY_COLOR_YELLOW : ANTBOY_COLOR_DARKGREY;
    AntBoy.Display.fillCircle(230, 150, 14, colB);
    AntBoy.Display.setTextColor(ANTBOY_COLOR_BLACK);
    AntBoy.Display.setCursor(227, 146);
    AntBoy.Display.print("B");

    // Suara feedback klik saat tombol A atau B ditekan
    if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
        AntBoy.Audio.playTone(2637, 30);
    }
    if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
        AntBoy.Audio.playTone(2093, 30);
    }
}

void showVolumeToast(const char* volStr) {
    AntBoy.Display.fillRoundRect(80, 100, 160, 40, 8, ANTBOY_COLOR_NAVY);
    AntBoy.Display.drawRoundRect(78, 98, 164, 44, 8, ANTBOY_COLOR_CYAN);
    
    char buf[32];
    snprintf(buf, sizeof(buf), "VOLUME: %s", volStr);
    AntBoy.Display.drawCenteredText(buf, 114, ANTBOY_COLOR_WHITE, 1);
    delay(400);
    drawDashboardBase();
}
