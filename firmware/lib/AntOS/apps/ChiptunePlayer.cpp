#include "ChiptunePlayer.h"

ChiptunePlayerClass ChiptunePlayer;

static const ChiptuneTrack BUILTIN_TRACKS[7] = {
    {
        "Tetris (Korobeiniki)",
        "Alexey Pajitnov / Game Boy",
        "Tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a"
    },
    {
        "Super Mario Bros",
        "Koji Kondo / Nintendo",
        "Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b"
    },
    {
        "Legend of Zelda",
        "Koji Kondo / Nintendo",
        "Zelda:d=4,o=5,b=125:a#,8f,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f6,16f#6,16g#6,2a#6,8g#6,8f#6,8g#6,16f#6,2f6,8p,8d#6,16d#6,16f6,2f#6,8f6,8d#6,8c#6,16c#6,16d#6,2f6,8d#6,8c#6,2c6"
    },
    {
        "Pac-Man Intro",
        "Toshio Kai / Namco",
        "Pacman:d=4,o=5,b=120:32b5,32b6,32f#6,32d#6,32b6,32f#6,16d#6,32c6,32c7,32g6,32e6,32c7,32g6,16e6,32b5,32b6,32f#6,32d#6,32b6,32f#6,16d#6,32d#6,32e6,32f6,32f6,32f#6,32g6,32g#6,32a6,16b6"
    },
    {
        "Mega Man 2 (Wily)",
        "Takashi Tateishi / Capcom",
        "MegaMan2:d=4,o=5,b=140:8d#4,8d#4,8f4,8f#4,8g#4,8a#4,8c#,8c#,8c,8a#4,8g#4,8f#4,8f4,8f4,8f#4,8g#4,8a#4,8c#,8d#,8d#,8c#,8a#4,8g#4,8f#4,8d#4,8d#4,8f4,8f#4,8g#4,8a#4,8c#,8c#"
    },
    {
        "Doom (E1M1)",
        "Bobby Prince / id Software",
        "Doom:d=4,o=5,b=140:16e3,16e3,16e4,16e3,16e3,16d4,16e3,16e3,16c4,16e3,16e3,16a#3,16e3,16e3,16b3,16c4,16e3,16e3,16e4,16e3,16e3,16d4,16e3,16e3,16c4,16e3,16e3,8a#3"
    },
    {
        "Pokemon Theme",
        "Junichi Masuda / Game Freak",
        "Pokemon:d=4,o=5,b=140:8g,8g,8g,8g,8d#,8f,8g,4p,8g,8g,8g,8g,8d#,8f,8g,4p,8g,8a#,8c6,8d6,8d#6,8d6,8c6,8a#,8g,8a#,8c6,8d6,2d#6,4f6,2g6"
    }
};

void ChiptunePlayerClass::updateVisualizer(uint16_t freq) {
    // Map frequency to 16 bands (range 100 Hz to 4000 Hz)
    int targetBand = 0;
    if (freq > 0) {
        targetBand = constrain((int)(log10(freq / 80.0) * 10.0), 0, 15);
        specBands[targetBand] = constrain(specBands[targetBand] + 28, 0, 36);
        if (targetBand > 0) specBands[targetBand - 1] = constrain(specBands[targetBand - 1] + 14, 0, 30);
        if (targetBand < 15) specBands[targetBand + 1] = constrain(specBands[targetBand + 1] + 14, 0, 30);
    }

    // Decay all bands
    for (int i = 0; i < 16; i++) {
        if (specBands[i] > 2) specBands[i] -= 2;
        else specBands[i] = 0;

        if (specBands[i] > specPeaks[i]) {
            specPeaks[i] = specBands[i];
        } else if (specPeaks[i] > 0) {
            specPeaks[i] -= 1;
        }
    }
}

void ChiptunePlayerClass::renderUI() {
    const ChiptuneTrack& trk = BUILTIN_TRACKS[currentTrackIndex];

    // Top Header
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 24, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 24, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 8);
    AntBoy.Display.print("< [B] EXIT");

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("CHIPTUNE 8-BIT PLAYER", 8, ANTOS_COLOR_WHITE, 1);

    // Track Info Card
    int cardY = 32;
    AntBoy.Display.fillRoundRect(12, cardY, 296, 68, 6, ANTOS_COLOR_BG_CARD_ACTIVE);
    AntBoy.Display.drawRoundRect(12, cardY, 296, 68, 6, ANTOS_COLOR_PIL_GAMING);

    // Track index badge [1/7]
    char idxBuf[16];
    snprintf(idxBuf, sizeof(idxBuf), "TRACK %d / 7", currentTrackIndex + 1);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_CARD_ACTIVE);
    AntBoy.Display.setCursor(20, cardY + 8);
    AntBoy.Display.print(idxBuf);

    // Status (PLAYING / PAUSED / STOPPED)
    const char* statusStr = isPlaying ? (isPaused ? "[PAUSED]" : "[PLAYING]") : "[STOPPED]";
    uint16_t statusCol = isPlaying ? (isPaused ? ANTOS_COLOR_YELLOW : ANTOS_COLOR_GREEN) : ANTOS_COLOR_RED;
    AntBoy.Display.setTextColor(statusCol, ANTOS_COLOR_BG_CARD_ACTIVE);
    AntBoy.Display.setCursor(225, cardY + 8);
    AntBoy.Display.print(statusStr);

    // Title
    AntBoy.Display.setTextSize(2);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_CARD_ACTIVE);
    AntBoy.Display.setCursor(20, cardY + 24);
    AntBoy.Display.print(trk.title);

    // Author
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_CARD_ACTIVE);
    AntBoy.Display.setCursor(20, cardY + 48);
    AntBoy.Display.print(trk.author);

    // Visualizer Panel (Spectrum Analyzer + Waveform)
    int visY = 108;
    int visH = 92;
    AntBoy.Display.fillRoundRect(12, visY, 296, visH, 6, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawRoundRect(12, visY, 296, visH, 6, ANTOS_COLOR_BORDER_DIM);

    // Center Oscilloscope Waveform Line
    int midY = visY + 24;
    AntBoy.Display.drawFastHLine(20, midY, 280, 0x18C3); // Faint center grid line
    for (int x = 0; x < 280; x += 2) {
        float rad = (x * 0.12f) + (millis() * 0.008f);
        int amp = isPlaying && !isPaused ? 10 : 1;
        int wy = midY + (int)(sin(rad) * amp);
        AntBoy.Display.drawPixel(20 + x, wy, ANTOS_COLOR_CYAN);
    }

    // 16 Frequency Bars
    int barW = 12;
    int barSpacing = 17;
    int startBarX = 20;
    int baseBarY = visY + visH - 8;

    for (int i = 0; i < 16; i++) {
        int barH = specBands[i];
        int barX = startBarX + (i * barSpacing);

        // Clear bar background
        AntBoy.Display.fillRect(barX, baseBarY - 38, barW, 38, ANTOS_COLOR_BG_DARK);

        if (barH > 0) {
            uint16_t bCol = (i < 5) ? ANTOS_COLOR_GREEN : ((i < 11) ? ANTOS_COLOR_YELLOW : ANTOS_COLOR_RED);
            AntBoy.Display.fillRect(barX, baseBarY - barH, barW, barH, bCol);
            AntBoy.Display.drawFastHLine(barX, baseBarY - barH, barW, ANTOS_COLOR_WHITE);
        }

        // Peak dot
        if (specPeaks[i] > 0) {
            AntBoy.Display.drawFastHLine(barX, baseBarY - specPeaks[i] - 1, barW, ANTOS_COLOR_MAGENTA);
        }
    }

    // Footer Controls
    int fy = ANTBOY_SCREEN_HEIGHT - 22;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, fy, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_BORDER_DIM);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("[A] Play/Pause | [^/v] Ganti Lagu | [B] Keluar", fy + 7, ANTOS_COLOR_WHITE, 1);
}

void ChiptunePlayerClass::run() {
    currentTrackIndex = 0;
    isPlaying = false;
    isPaused = false;
    for (int i = 0; i < 16; i++) {
        specBands[i] = 0;
        specPeaks[i] = 0;
    }

    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    renderUI();

    bool inPlayer = true;

    while (inPlayer) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            AntBoy.Audio.stop();
            break;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
            AntBoy.Audio.stop();
            currentTrackIndex = (currentTrackIndex > 0) ? currentTrackIndex - 1 : 6;
            isPlaying = false;
            isPaused = false;
            AntBoy.Audio.playTone(2637, 15);
            renderUI();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
            AntBoy.Audio.stop();
            currentTrackIndex = (currentTrackIndex + 1) % 7;
            isPlaying = false;
            isPaused = false;
            AntBoy.Audio.playTone(2637, 15);
            renderUI();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            if (!isPlaying) {
                isPlaying = true;
                isPaused = false;
                renderUI();
                playCurrentTrack();
                isPlaying = false;
                renderUI();
            } else {
                isPaused = !isPaused;
                if (isPaused) AntBoy.Audio.stop();
                renderUI();
            }
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            AntBoy.Audio.stop();
            isPlaying = false;
            isPaused = false;
            inPlayer = false;
        }

        updateVisualizer(0);
        delay(25);
    }
}

void ChiptunePlayerClass::playCurrentTrack() {
    const char* rtttl = BUILTIN_TRACKS[currentTrackIndex].rtttl;
    if (!rtttl) return;

    // Parse RTTTL and play note-by-note while updating visualizer
    const char* p = rtttl;
    while (*p && *p != ':') p++;
    if (*p == ':') p++;

    int default_dur = 4;
    int default_oct = 6;
    int bpm = 100;

    // Header params
    while (*p && *p != ':') {
        char param = *p++;
        if (*p == '=') p++;
        int val = 0;
        while (*p >= '0' && *p <= '9') {
            val = val * 10 + (*p++ - '0');
        }
        if (param == 'd') default_dur = val;
        else if (param == 'o') default_oct = val;
        else if (param == 'b') bpm = val;
        if (*p == ',') p++;
    }
    if (*p == ':') p++;

    long wholenote = (60000L * 4) / bpm;

    while (*p && isPlaying) {
        AntBoy.update();
        if (AntBoy.checkExitShortcut() || AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            AntBoy.Audio.stop();
            isPlaying = false;
            break;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            isPaused = !isPaused;
            if (isPaused) {
                AntBoy.Audio.stop();
                renderUI();
                while (isPaused) {
                    AntBoy.update();
                    if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) isPaused = false;
                    if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) { isPlaying = false; break; }
                    delay(30);
                }
                renderUI();
            }
        }

        // Note duration
        int dur = 0;
        while (*p >= '0' && *p <= '9') {
            dur = dur * 10 + (*p++ - '0');
        }
        if (dur == 0) dur = default_dur;

        // Note note
        char note = *p++;
        bool sharp = false;
        if (*p == '#') {
            sharp = true;
            p++;
        }

        // Dot
        bool dotted = false;
        if (*p == '.') {
            dotted = true;
            p++;
        }

        // Scale / Octave
        int scale = default_oct;
        if (*p >= '0' && *p <= '9') {
            scale = *p++ - '0';
        }

        if (*p == ',') p++;

        long duration = wholenote / dur;
        if (dotted) duration += duration / 2;

        // Frequency table
        static const uint16_t notes[12] = {
            262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
        };
        int noteIndex = -1;
        switch (note) {
            case 'c': noteIndex = 0; break;
            case 'd': noteIndex = 2; break;
            case 'e': noteIndex = 4; break;
            case 'f': noteIndex = 5; break;
            case 'g': noteIndex = 7; break;
            case 'a': noteIndex = 9; break;
            case 'b': noteIndex = 11; break;
            case 'p': noteIndex = -1; break;
        }
        if (noteIndex >= 0 && sharp) noteIndex++;

        uint16_t freq = 0;
        if (noteIndex >= 0) {
            freq = notes[noteIndex];
            int shift = scale - 4;
            if (shift > 0) freq <<= shift;
            else if (shift < 0) freq >>= -shift;
        }

        if (freq > 0) {
            AntBoy.Audio.playTone(freq, (uint32_t)(duration * 0.85));
            updateVisualizer(freq);
        } else {
            AntBoy.Audio.stop();
            updateVisualizer(0);
        }

        // Step animation during note
        uint32_t startNote = millis();
        while (millis() - startNote < duration && isPlaying) {
            updateVisualizer(freq);
            renderUI();
            delay(20);
        }
    }

    AntBoy.Audio.stop();
}
