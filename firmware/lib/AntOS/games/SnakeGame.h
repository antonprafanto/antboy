#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

class SnakeGameClass {
public:
    void run();

private:
    static const int GRID_W = 20;
    static const int GRID_H = 14;
    static const int CELL_SIZE = 15;
    static const int OFFSET_X = 10;
    static const int OFFSET_Y = 24;

    struct Point {
        int8_t x;
        int8_t y;
    };

    Point snake[200];
    int snakeLen;
    int dirX, dirY;
    int nextDirX, nextDirY;
    Point food;
    int score;
    int highScore;
    bool gameOver;

    void resetGame();
    void spawnFood();
    void updateLogic();
    void render();
    void loadHighScore();
    void saveHighScore();
};

extern SnakeGameClass SnakeGame;
