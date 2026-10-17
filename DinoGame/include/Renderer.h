#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>

class Player;
class Obstacle;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void drawFrame(const Player& player,
                   const std::vector<Obstacle*>& obstacles,
                   int score, int highScore) const;

    void drawStartScreen() const;
    void drawGameOver(int score, int highScore) const;

private:
    static constexpr int SCREEN_WIDTH = 60;
    static constexpr int SCREEN_HEIGHT = 15;
    static constexpr int GROUND_ROW = 12;

    mutable bool needsFullClear;

    struct Cell {
        char ch;
        const char* colorCode;
    };

    void clearScreen() const;
    void drawGround(std::vector<Cell>& buffer, int score) const;
    void drawPlayer(std::vector<Cell>& buffer, const Player& player) const;
    void drawObstacles(std::vector<Cell>& buffer,
                       const std::vector<Obstacle*>& obstacles) const;
    void drawScore(std::vector<Cell>& buffer, int score, int highScore) const;
    void drawClouds(std::vector<Cell>& buffer, int score) const;

    void setCharAt(std::vector<Cell>& buffer, int row, int col, char ch, const char* colorCode) const;
    int getIndex(int row, int col) const;
};

#endif
