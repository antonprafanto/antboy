#include "TetrisGame.h"
#include <Preferences.h>
#include "../AntOS_PauseModal.h"

TetrisGameClass TetrisGame;

// 7 Tetromino shapes in 4x4 bounding box (4 rotations each)
static const uint16_t TETROMINOES[7][4] = {
    // I (Cyan)
    { 0x0F00, 0x2222, 0x00F0, 0x4444 },
    // J (Blue)
    { 0x8E00, 0x6440, 0x0E20, 0x44C0 },
    // L (Orange)
    { 0x2E00, 0x4460, 0x0E80, 0xC440 },
    // O (Yellow)
    { 0x6600, 0x6600, 0x6600, 0x6600 },
    // S (Green)
    { 0x6C00, 0x4620, 0x06C0, 0x8C40 },
    // T (Magenta)
    { 0x4E00, 0x4640, 0x0E40, 0x4C40 },
    // Z (Red)
    { 0xC600, 0x2640, 0x0C60, 0x4C80 }
};

static const uint16_t TETRO_COLORS[7] = {
    ANTOS_COLOR_CYAN,
    ANTOS_COLOR_BLUE,
    0xFD20, // Orange
    ANTOS_COLOR_YELLOW,
    ANTOS_COLOR_GREEN,
    ANTOS_COLOR_MAGENTA,
    ANTOS_COLOR_RED
};

void TetrisGameClass::loadHighScore() {
    Preferences prefs;
    prefs.begin("antboy_tetris", true);
    highScore = prefs.getInt("hi", 0);
    prefs.end();
}

void TetrisGameClass::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        Preferences prefs;
        prefs.begin("antboy_tetris", false);
        prefs.putInt("hi", highScore);
        prefs.end();
    }
}

void TetrisGameClass::resetGame() {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            board[y][x] = 0;
        }
    }
    score = 0;
    linesCleared = 0;
    level = 1;
    gameOver = false;
    nextType = random(0, 7);
    spawnPiece();
}

void TetrisGameClass::spawnPiece() {
    curType = nextType;
    nextType = random(0, 7);
    curRot = 0;
    curX = (BOARD_W / 2) - 2;
    curY = 0;

    if (checkCollision(curType, curRot, curX, curY)) {
        gameOver = true;
        AntBoy.Audio.playError();
        saveHighScore();
    }
}

bool TetrisGameClass::checkCollision(int type, int rot, int x, int y) {
    uint16_t mask = TETROMINOES[type][rot];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (mask & (0x8000 >> (r * 4 + c))) {
                int bx = x + c;
                int by = y + r;
                if (bx < 0 || bx >= BOARD_W || by >= BOARD_H) {
                    return true;
                }
                if (by >= 0 && board[by][bx] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void TetrisGameClass::lockPiece() {
    uint16_t mask = TETROMINOES[curType][curRot];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (mask & (0x8000 >> (r * 4 + c))) {
                int bx = curX + c;
                int by = curY + r;
                if (by >= 0 && by < BOARD_H && bx >= 0 && bx < BOARD_W) {
                    board[by][bx] = TETRO_COLORS[curType];
                }
            }
        }
    }
    AntBoy.Audio.playTone(880, 15);
    clearLines();
    spawnPiece();
}

void TetrisGameClass::clearLines() {
    int cleared = 0;
    for (int y = BOARD_H - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < BOARD_W; x++) {
            if (board[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            cleared++;
            for (int ty = y; ty > 0; ty--) {
                for (int tx = 0; tx < BOARD_W; tx++) {
                    board[ty][tx] = board[ty - 1][tx];
                }
            }
            for (int tx = 0; tx < BOARD_W; tx++) {
                board[0][tx] = 0;
            }
            y++; // Check same row again
        }
    }

    if (cleared > 0) {
        linesCleared += cleared;
        level = (linesCleared / 10) + 1;
        int pts = 0;
        switch (cleared) {
            case 1: pts = 100 * level; break;
            case 2: pts = 300 * level; break;
            case 3: pts = 500 * level; break;
            case 4: pts = 800 * level; break;
        }
        score += pts;

        // Suara chime line clear
        if (cleared == 4) {
            AntBoy.Audio.playTone(1318, 50); // E6
            delay(40);
            AntBoy.Audio.playTone(2093, 80); // C7
        } else {
            AntBoy.Audio.playTone(1568, 40); // G6
        }
    }
}

void TetrisGameClass::rotatePiece(bool clockwise) {
    int newRot = clockwise ? (curRot + 1) % 4 : (curRot + 3) % 4;
    if (!checkCollision(curType, newRot, curX, curY)) {
        curRot = newRot;
        AntBoy.Audio.playTone(1760, 10);
    } else if (!checkCollision(curType, newRot, curX - 1, curY)) { // Wall kick left
        curX--;
        curRot = newRot;
        AntBoy.Audio.playTone(1760, 10);
    } else if (!checkCollision(curType, newRot, curX + 1, curY)) { // Wall kick right
        curX++;
        curRot = newRot;
        AntBoy.Audio.playTone(1760, 10);
    }
}

void TetrisGameClass::dropPiece() {
    if (!checkCollision(curType, curRot, curX, curY + 1)) {
        curY++;
    } else {
        lockPiece();
    }
}

void TetrisGameClass::hardDrop() {
    while (!checkCollision(curType, curRot, curX, curY + 1)) {
        curY++;
        score += 2;
    }
    lockPiece();
}

void TetrisGameClass::render() {
    // 1. Gambar Area Papan Permainan (Tengah)
    int boardPixelW = BOARD_W * BLOCK_SIZE;
    int boardPixelH = BOARD_H * BLOCK_SIZE;
    AntBoy.Display.drawRect(BOARD_X - 1, BOARD_Y - 1, boardPixelW + 2, boardPixelH + 2, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.fillRect(BOARD_X, BOARD_Y, boardPixelW, boardPixelH, ANTOS_COLOR_BG_DARK);

    // Grid halus
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            if (board[y][x] != 0) {
                int px = BOARD_X + (x * BLOCK_SIZE);
                int py = BOARD_Y + (y * BLOCK_SIZE);
                AntBoy.Display.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, board[y][x]);
                AntBoy.Display.drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, ANTOS_COLOR_WHITE);
            }
        }
    }

    // Ghost piece
    int ghostY = curY;
    while (!checkCollision(curType, curRot, curX, ghostY + 1)) {
        ghostY++;
    }
    uint16_t mask = TETROMINOES[curType][curRot];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (mask & (0x8000 >> (r * 4 + c))) {
                int gx = BOARD_X + ((curX + c) * BLOCK_SIZE);
                int gy = BOARD_Y + ((ghostY + r) * BLOCK_SIZE);
                if (ghostY != curY) {
                    AntBoy.Display.drawRect(gx, gy, BLOCK_SIZE, BLOCK_SIZE, 0x4208); // Dim ghost outline
                }
            }
        }
    }

    // Current active piece
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (mask & (0x8000 >> (r * 4 + c))) {
                int px = BOARD_X + ((curX + c) * BLOCK_SIZE);
                int py = BOARD_Y + ((curY + r) * BLOCK_SIZE);
                AntBoy.Display.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, TETRO_COLORS[curType]);
                AntBoy.Display.drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, ANTOS_COLOR_WHITE);
            }
        }
    }

    // 2. Panel Kiri: Skor & Level
    AntBoy.Display.fillRoundRect(10, BOARD_Y, 85, 110, 4, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(10, BOARD_Y, 85, 110, 4, ANTOS_COLOR_CYAN);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 8);
    AntBoy.Display.print("TETRIS");
    AntBoy.Display.setCursor(16, BOARD_Y + 18);
    AntBoy.Display.print("POCKET");

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 36);
    AntBoy.Display.print("SCORE:");
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 48);
    AntBoy.Display.printf("%d", score);

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 64);
    AntBoy.Display.print("LEVEL:");
    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 76);
    AntBoy.Display.printf("%d", level);

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(16, BOARD_Y + 92);
    AntBoy.Display.printf("LINES:%d", linesCleared);

    // 3. Panel Kanan: Next Piece & High Score
    AntBoy.Display.fillRoundRect(225, BOARD_Y, 85, 110, 4, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(225, BOARD_Y, 85, 110, 4, ANTOS_COLOR_MAGENTA);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_MAGENTA, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(232, BOARD_Y + 8);
    AntBoy.Display.print("NEXT PIECE");

    // Preview next piece
    uint16_t nextMask = TETROMINOES[nextType][0];
    int prevStartX = 245;
    int prevStartY = BOARD_Y + 28;
    AntBoy.Display.fillRect(prevStartX - 10, prevStartY - 5, 55, 45, ANTOS_COLOR_BG_DARK);
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (nextMask & (0x8000 >> (r * 4 + c))) {
                int px = prevStartX + (c * 9);
                int py = prevStartY + (r * 9);
                AntBoy.Display.fillRect(px, py, 9, 9, TETRO_COLORS[nextType]);
                AntBoy.Display.drawRect(px, py, 9, 9, ANTOS_COLOR_WHITE);
            }
        }
    }

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(232, BOARD_Y + 80);
    AntBoy.Display.print("HIGH SCORE:");
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(232, BOARD_Y + 94);
    AntBoy.Display.printf("%d", highScore);

    // Footer Info
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawCenteredText("[A] Putar  [^] Drop  [B] Kembali", 232, ANTOS_COLOR_TEXT_DIM, 1);

    if (gameOver) {
        int modalW = 200;
        int modalH = 80;
        int mx = (ANTBOY_SCREEN_WIDTH - modalW) / 2;
        int my = (ANTBOY_SCREEN_HEIGHT - modalH) / 2;
        AntBoy.Display.fillRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_RED);

        AntBoy.Display.setTextSize(2);
        AntBoy.Display.setTextColor(ANTOS_COLOR_RED, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("GAME OVER!", my + 14, ANTOS_COLOR_RED, 1);

        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("[A] Main Lagi  |  [B] Keluar", my + 48, ANTOS_COLOR_WHITE, 1);
    }
}

void TetrisGameClass::run() {
    loadHighScore();
    resetGame();
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    AntBoy.Audio.playTone(1975, 40); // B6

    uint32_t lastDrop = millis();
    bool running = true;

    while (running) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            break;
        }

        // Tombol MENU atau SELECT membuka Pause Menu
        if (AntBoy.Buttons.wasPressed(ANT_BTN_MENU) || (!gameOver && AntBoy.Buttons.wasPressed(ANT_BTN_SELECT))) {
            if (!AntOS_ShowPauseMenu("TETRIS POCKET")) {
                running = false;
                break;
            }
            AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
            render();
            lastDrop = millis();
        }

        if (!gameOver) {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_LEFT)) {
                if (!checkCollision(curType, curRot, curX - 1, curY)) {
                    curX--;
                    AntBoy.Audio.playTone(1200, 10);
                }
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_RIGHT)) {
                if (!checkCollision(curType, curRot, curX + 1, curY)) {
                    curX++;
                    AntBoy.Audio.playTone(1200, 10);
                }
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
                dropPiece();
                score += 1;
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
                hardDrop();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                rotatePiece(true);
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
                rotatePiece(false);
            }
        } else {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                resetGame();
                lastDrop = millis();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B) || AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
                running = false;
                break;
            }
        }

        // Kecepatan drop otomatis berdasarkan level
        uint32_t dropInterval = (level < 10) ? (600 - (level * 45)) : 150;
        if (!gameOver && (millis() - lastDrop >= dropInterval)) {
            lastDrop = millis();
            dropPiece();
        }

        render();
        delay(20);
    }

    saveHighScore();
}
