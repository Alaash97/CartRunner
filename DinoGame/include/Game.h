#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Player.h"
#include "Obstacle.h"
#include "Renderer.h"

class Game
{
public:
    Game();
    ~Game();

    void run();

private:
    static const int FRAME_TIME_MS = 50;
    static const int SCREEN_WIDTH = 60;
    static const float OBSTACLE_SPEED;
    static const int MIN_SPAWN_INTERVAL = 20;
    static const int MAX_SPAWN_INTERVAL = 50;

    Player player;
    Renderer renderer;
    std::vector<Obstacle*> obstacles;
    int score;
    int highScore;
    bool running;
    int framesSinceLastSpawn;
    int nextSpawnInterval;

    void handleInput(int key);
    void updateGame();
    void spawnObstacle();
    void removeOffScreenObstacles();
    bool checkCollisions() const;
    void cleanup();
    int randomInterval() const;
    void loadHighScore();
    void saveHighScore() const;
};

#endif
