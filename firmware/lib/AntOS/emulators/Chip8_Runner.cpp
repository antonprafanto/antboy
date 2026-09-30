#include "Chip8_Runner.h"

Chip8_RunnerClass Chip8_Runner;

static const uint8_t CHIP8_FONTSET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Built-in Classic Pong ROM for CHIP-8 (Paul Vervalin 1990)
static const uint8_t CHIP8_PONG_ROM[] = {
    0x6A, 0x02, 0x6B, 0x0C, 0x6C, 0x3F, 0x6D, 0x0C, 0xA2, 0xEA, 0xDA, 0xB6, 0xDC, 0xD6, 0x6E, 0x00,
    0x22, 0xD4, 0x66, 0x03, 0x68, 0x02, 0x60, 0x60, 0xF0, 0x15, 0xF0, 0x07, 0x30, 0x00, 0x12, 0x1A,
    0xC7, 0x17, 0x77, 0x08, 0x69, 0xFF, 0xA2, 0xF0, 0xD6, 0x71, 0xA2, 0xEA, 0xDA, 0xB6, 0xDC, 0xD6,
    0x60, 0x01, 0xE0, 0xA1, 0x7B, 0xFE, 0x60, 0x04, 0xE0, 0xA1, 0x7B, 0x02, 0x60, 0x1F, 0x8B, 0x02,
    0xDA, 0xB6, 0x60, 0x0C, 0xE0, 0xA1, 0x7D, 0xFE, 0x60, 0x0D, 0xE0, 0xA1, 0x7D, 0x02, 0x60, 0x1F,
    0x8D, 0x02, 0xDC, 0xD6, 0xA2, 0xF0, 0xD6, 0x71, 0x86, 0x84, 0x87, 0x94, 0x60, 0x3F, 0x86, 0x02,
    0x61, 0x1F, 0x87, 0x12, 0x36, 0x00, 0x12, 0x6E, 0x46, 0x3F, 0x12, 0x7C, 0x12, 0x86, 0x60, 0x02,
    0xFE, 0x18, 0x12, 0x86, 0x60, 0x00, 0x61, 0x00, 0xA2, 0xF2, 0xD0, 0x12, 0x12, 0x86, 0x12, 0x86,
    0x80, 0x60, 0x3F, 0x01, 0x12, 0x92, 0x61, 0x02, 0x80, 0x15, 0x3F, 0x01, 0x12, 0x9E, 0x80, 0x70,
    0x80, 0xD5, 0x3F, 0x01, 0x12, 0xA6, 0x12, 0x12, 0x68, 0xFE, 0x62, 0x20, 0xA2, 0xF2, 0x60, 0x05,
    0x22, 0xD4, 0x12, 0x12, 0x66, 0x02, 0x68, 0xFE, 0x62, 0x20, 0xA2, 0xF2, 0x60, 0x05, 0x22, 0xD4,
    0x12, 0x12, 0x68, 0x02, 0x63, 0x00, 0x22, 0xC6, 0x12, 0x12, 0x68, 0xFE, 0x63, 0x00, 0x22, 0xC6,
    0x12, 0x12, 0x80, 0x60, 0x3F, 0x01, 0x12, 0xD0, 0x61, 0x02, 0x80, 0x15, 0x3F, 0x01, 0x12, 0xDE,
    0x80, 0x70, 0x80, 0xB5, 0x3F, 0x01, 0x12, 0xE6, 0x12, 0x12, 0x80, 0x80, 0x40, 0x00, 0x12, 0xFA,
    0x80, 0x00, 0x40, 0x00, 0x12, 0xFA, 0x00, 0xEE
};

void Chip8_RunnerClass::reset() {
    pc = 0x200;
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = true;
    isRunning = true;

    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(gfx, 0, sizeof(gfx));
    memset(stack, 0, sizeof(stack));
    memset(key, 0, sizeof(key));

    // Load fontset into memory 0x00 to 0x50
    for (int i = 0; i < 80; i++) {
        memory[i] = CHIP8_FONTSET[i];
    }
}

void Chip8_RunnerClass::loadBuiltinRom(int gameIdx) {
    reset();
    for (size_t i = 0; i < sizeof(CHIP8_PONG_ROM); i++) {
        memory[0x200 + i] = CHIP8_PONG_ROM[i];
    }
}

void Chip8_RunnerClass::emulateCycle() {
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    uint16_t nnn = opcode & 0x0FFF;
    uint8_t  nn  = opcode & 0x00FF;
    uint8_t  n   = opcode & 0x000F;
    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) { // CLS
                memset(gfx, 0, sizeof(gfx));
                drawFlag = true;
            } else if (opcode == 0x00EE) { // RET
                if (sp > 0) pc = stack[--sp];
            }
            break;

        case 0x1000: pc = nnn; break; // JP addr
        case 0x2000: stack[sp++] = pc; pc = nnn; break; // CALL addr
        case 0x3000: if (V[x] == nn) pc += 2; break; // SE Vx, byte
        case 0x4000: if (V[x] != nn) pc += 2; break; // SNE Vx, byte
        case 0x5000: if (V[x] == V[y]) pc += 2; break; // SE Vx, Vy
        case 0x6000: V[x] = nn; break; // LD Vx, byte
        case 0x7000: V[x] += nn; break; // ADD Vx, byte

        case 0x8000:
            switch (n) {
                case 0x0: V[x] = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4: {
                    uint16_t sum = (uint16_t)V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                    break;
                }
                case 0x5:
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6:
                    V[0xF] = V[x] & 1;
                    V[x] >>= 1;
                    break;
                case 0x7:
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    V[0xF] = (V[x] >> 7) & 1;
                    V[x] <<= 1;
                    break;
            }
            break;

        case 0x9000: if (V[x] != V[y]) pc += 2; break;
        case 0xA000: I = nnn; break; // LD I, addr
        case 0xB000: pc = nnn + V[0]; break;
        case 0xC000: V[x] = random(0, 256) & nn; break; // RND Vx, byte

        case 0xD000: { // DRW Vx, Vy, nibble (Draw sprite)
            uint8_t vx = V[x] % 64;
            uint8_t vy = V[y] % 32;
            V[0xF] = 0;

            for (int row = 0; row < n; row++) {
                uint8_t pixel = memory[I + row];
                for (int col = 0; col < 8; col++) {
                    if ((pixel & (0x80 >> col)) != 0) {
                        int px = (vx + col) % 64;
                        int py = (vy + row) % 32;
                        int idx = py * 64 + px;
                        if (gfx[idx] == 1) V[0xF] = 1;
                        gfx[idx] ^= 1;
                    }
                }
            }
            drawFlag = true;
            break;
        }

        case 0xE000:
            if (nn == 0x9E) { // SKP Vx
                if (key[V[x] & 0x0F]) pc += 2;
            } else if (nn == 0xA1) { // SKNP Vx
                if (!key[V[x] & 0x0F]) pc += 2;
            }
            break;

        case 0xF000:
            switch (nn) {
                case 0x07: V[x] = delay_timer; break;
                case 0x0A: { // Wait for key
                    bool pressed = false;
                    for (int k = 0; k < 16; k++) {
                        if (key[k]) { V[x] = k; pressed = true; break; }
                    }
                    if (!pressed) pc -= 2;
                    break;
                }
                case 0x15: delay_timer = V[x]; break;
                case 0x18: sound_timer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = V[x] * 5; break; // Font sprite addr
                case 0x33: // BCD
                    memory[I]     = V[x] / 100;
                    memory[I + 1] = (V[x] / 10) % 10;
                    memory[I + 2] = V[x] % 10;
                    break;
                case 0x55:
                    for (int i = 0; i <= x; i++) memory[I + i] = V[i];
                    break;
                case 0x65:
                    for (int i = 0; i <= x; i++) V[i] = memory[I + i];
                    break;
            }
            break;
    }

    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) {
        sound_timer--;
        AntBoy.Audio.playTone(880, 10);
    }
}

void Chip8_RunnerClass::renderDisplay() {
    if (!drawFlag) return;
    drawFlag = false;

    // Skala 64x32 ke layar 320x160 (Faktor skala 5x: 64*5 = 320, 32*5 = 160)
    // Ditampilkan di Y=40 s.d. 200
    int startY = 40;
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            uint16_t col = gfx[y * 64 + x] ? ANTOS_COLOR_GREEN : ANTOS_COLOR_BG_DARK;
            AntBoy.Display.fillRect(x * 5, startY + (y * 5), 5, 5, col);
        }
    }
}

void Chip8_RunnerClass::updateInput() {
    // Keypad mapping ANTBOY -> CHIP-8
    // Pad 1 (Key 1): UP
    // Pad 4 (Key 4): DOWN
    // Pad A (Key 5): Button A
    // Pad B (Key 6): Button B
    key[1] = AntBoy.Buttons.isPressed(ANT_BTN_UP);
    key[4] = AntBoy.Buttons.isPressed(ANT_BTN_DOWN);
    key[7] = AntBoy.Buttons.isPressed(ANT_BTN_LEFT);
    key[8] = AntBoy.Buttons.isPressed(ANT_BTN_RIGHT);
    key[5] = AntBoy.Buttons.isPressed(ANT_BTN_A);
    key[6] = AntBoy.Buttons.isPressed(ANT_BTN_B);
    key[0] = AntBoy.Buttons.isPressed(ANT_BTN_SELECT);
}

void Chip8_RunnerClass::run() {
    loadBuiltinRom(0);

    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);

    // Header
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 26, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 26, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 9);
    AntBoy.Display.print("< [B] BACK");
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("CHIP-8 & ATARI RETRO VM", 9, ANTOS_COLOR_WHITE, 1);

    // Frame border di sekeliling 320x160
    AntBoy.Display.drawRect(0, 39, 320, 162, ANTOS_COLOR_PIL_GAMING);

    // Footer
    int fy = ANTBOY_SCREEN_HEIGHT - 20;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 20, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("[^/v] Paddle  |  SELECT + START: Exit", fy + 6, ANTOS_COLOR_TEXT_DIM, 1);

    while (isRunning) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut() || AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            isRunning = false;
            break;
        }

        updateInput();

        // 8 CPU cycles per frame (~480Hz execution speed)
        for (int i = 0; i < 8; i++) {
            emulateCycle();
        }

        renderDisplay();
        delay(16); // ~60 FPS
    }
}
