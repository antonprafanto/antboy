#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

class TetrisGameClass {
public:
    void run();

private:
    static const int BOARD_W = 10;
    static const int BOARD_H = 20;
    static const int BLOCK_SIZE = 11;
    static const int BOARD_X = 105;
    static const int BOARD_Y = 10;

    uint16_t board[BOARD_H][BOARD_W];

    int curType;
    int curRot;
    int curX, curY;

    int nextType;
    int score;
    int linesCleared;
    int level;
    int highScore;
    bool gameOver;

    void resetGame();
    void spawnPiece();
    bool checkCollision(int type, int rot, int x, int y);
    void lockPiece();
    void clearLines();
    void rotatePiece(bool clockwise);
    void dropPiece();
    void hardDrop();
    void render();
    void loadHighScore();
    void saveHighScore();
};

extern TetrisGameClass TetrisGame;
