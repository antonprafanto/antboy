#include "AntOS_Splash.h"

AntOS_SplashClass AntOS_Splash;

void AntOS_SplashClass::drawBootLogo() {
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);

    // Garis grid horizontal tipis khas tampilan cyber retro
    for (int y = 0; y < ANTBOY_SCREEN_HEIGHT; y += 12) {
        AntBoy.Display.drawFastHLine(0, y, ANTBOY_SCREEN_WIDTH, 0x0841);
    }

    // Kotak bingkai neon logo ANTBOY
    int boxX = 35;
    int boxY = 32;
    int boxW = 250;
    int boxH = 75;

    AntBoy.Display.fillRoundRect(boxX, boxY, boxW, boxH, 8, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(boxX, boxY, boxW, boxH, 8, ANTOS_COLOR_CYAN);
    AntBoy.Display.drawRoundRect(boxX + 2, boxY + 2, boxW - 4, boxH - 4, 6, ANTOS_COLOR_BORDER_GLOW);

    // Teks Logo ANTBOY
    AntBoy.Display.setTextSize(3);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(boxX + 22, boxY + 14);
    AntBoy.Display.print("ANT");
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.print("BOY");
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.print(" 2026");

    // Subjudul Cyber Handheld
    AntBoy.Display.drawFastHLine(boxX + 15, boxY + 45, boxW - 30, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(boxX + 22, boxY + 54);
    AntBoy.Display.print("MULTI-PURPOSE CYBER CONSOLE");

    // Kredit Designer
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_MUTED, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawCenteredText("Hardware by Anton Prafanto | s.id/antonprafanto", 125, ANTOS_COLOR_TEXT_MUTED, 1);

    // Wadah Progress Bar
    int barX = 40;
    int barY = 150;
    int barW = 240;
    int barH = 14;

    AntBoy.Display.fillRoundRect(barX, barY, barW, barH, 4, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(barX, barY, barW, barH, 4, ANTOS_COLOR_BORDER_DIM);
}

void AntOS_SplashClass::updateProgress(uint8_t percent, const char* statusMsg) {
    int barX = 40;
    int barY = 150;
    int barW = 240;
    int barH = 14;

    if (percent > 100) percent = 100;
    int fillW = (barW - 4) * percent / 100;

    if (fillW > 0) {
        AntBoy.Display.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, ANTOS_COLOR_GREEN);
    }

    // Status Log Pesan di bawah progress bar
    AntBoy.Display.fillRect(20, 175, 280, 16, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawCenteredText(statusMsg, 175, ANTOS_COLOR_CYAN, 1);
}

void AntOS_SplashClass::run() {
    drawBootLogo();

    // 1. Cek CPU & RAM
    updateProgress(20, "CPU: ESP32 240MHz Dual-Core OK");
    delay(200);

    // 2. Cek Hardware Audio & Mainkan Startup Jingle
    updateProgress(45, "Audio Engine: Resonant Piezo Ready");
    AntBoy.Audio.playStartupJingle();

    // 3. Cek MicroSD FAT32 Storage
    if (AntBoy.SD.isMounted()) {
        char sdBuf[48];
        snprintf(sdBuf, sizeof(sdBuf), "MicroSD: %s (%llu MB) Mounted", 
            AntBoy.SD.cardTypeString(), AntBoy.SD.totalBytes() / (1024 * 1024));
        updateProgress(75, sdBuf);
    } else {
        updateProgress(75, "MicroSD: No Card (Native SPIFFS Mode)");
    }
    delay(250);

    // 4. Inisialisasi Kernel AntOS Selesai
    updateProgress(100, "AntOS v1.0 Kernel Ready. Starting UI...");
    delay(350);

    // Efek Wipe Transisi ke Launcher
    for (int y = 0; y < ANTBOY_SCREEN_HEIGHT; y += 8) {
        AntBoy.Display.fillRect(0, y, ANTBOY_SCREEN_WIDTH, 8, ANTOS_COLOR_BG_DARK);
        delay(6);
    }
}
