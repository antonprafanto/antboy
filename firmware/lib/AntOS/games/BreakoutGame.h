#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

class BreakoutGameClass {
public:
    void run();

private:
    static const int BRICK_ROWS = 5;
    static const int BRICK_COLS = 8;
    static const int BRICK_W = 34;
    static const int BRICK_H = 10;
    static const int BRICK_START_X = 24;
    static const int BRICK_START_Y = 35;

    bool bricks[BRICK_ROWS][BRICK_COLS];
    int bricksLeft;

    float paddleX;
    int paddleW;

    float ballX, ballY;
    float ballVX, ballVY;
    bool ballAttached;

    int lives;
    int score;
    int highScore;
    bool gameOver;
    bool gameWon;

    void resetGame();
    void resetBall();
    void initBricks();
    void updateLogic();
    void render();
    void loadHighScore();
    void saveHighScore();
};

extern BreakoutGameClass BreakoutGame;
