#include "Game.h"

const float Game::OBSTACLE_SPEED = 2.0f;

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <algorithm>

Game::Game()
    : score(0),
      highScore(0),
      running(false),
      framesSinceLastSpawn(0),
      nextSpawnInterval(MIN_SPAWN_INTERVAL)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadHighScore();
}

Game::~Game()
{
    saveHighScore();
    cleanup();
}

void Game::run()
{
    renderer.drawStartScreen();

    // Wait for player to press SPACE to start
    while (true)
    {
        if (_kbhit())
        {
            int key = _getch();

            if (key == ' ')
            {
                break;
            }
            if (key == 'q' || key == 'Q')
            {
                return;
            }
        }

        Sleep(FRAME_TIME_MS);
    }

    // Main game loop with restart support
    bool wantsToPlay = true;

    while (wantsToPlay)
    {
        // Reset game state for a new round
        cleanup();
        player = Player();
        score = 0;
        framesSinceLastSpawn = 0;
        nextSpawnInterval = randomInterval();
        running = true;

        // Core game loop
        while (running)
        {
            auto frameStart = std::chrono::steady_clock::now();

            // Handle input (non-blocking)
            if (_kbhit())
            {
                int key = _getch();
                handleInput(key);
            }

            // Update game state
            updateGame();

            // Update high score
            if (score > highScore)
            {
                highScore = score;
            }

            // Check collisions
            if (checkCollisions())
            {
                // Play crash sound
                Beep(150, 300);
                running = false;
                break;
            }

            // Render the frame
            renderer.drawFrame(player, obstacles, score, highScore);

            // Dynamically decrease frame time based on score for progression (difficulty)
            int currentFrameTime = (std::max)(20, FRAME_TIME_MS - (score / 100));

            // Control frame rate
            auto frameEnd = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                frameEnd - frameStart).count();

            if (elapsed < currentFrameTime)
            {
                Sleep(static_cast<DWORD>(currentFrameTime - elapsed));
            }
        }

        saveHighScore();

        // Show game over screen
        renderer.drawGameOver(score, highScore);

        // Wait for restart or quit
        while (true)
        {
            if (_kbhit())
            {
                int key = _getch();

                if (key == ' ')
                {
                    break;
                }
                if (key == 'q' || key == 'Q')
                {
                    wantsToPlay = false;
                    break;
                }
            }

            Sleep(FRAME_TIME_MS);
        }
    }
}

void Game::handleInput(int key)
{
    if (key == ' ')
    {
        if (player.isOnGround())
        {
            // Play a quick jump sound asynchronously
            // To prevent blocking, we can use a separate thread or just a very fast Beep
            // std::thread([](){ Beep(500, 100); }).detach(); is best for non-blocking
            // But we will just use a fast beep directly
            // Beep(Frequency, Duration)
            // Note: direct Beep might slightly block the main thread, 
            // so we'll keep it short or just change jump state and let it rip.
            player.jump();
            // A short beep mimicking jump sound
            Beep(600, 50); 
        }
    }
    else if (key == 'q' || key == 'Q')
    {
        running = false;
    }
}

void Game::updateGame()
{
    player.update();

    // Update all obstacles
    for (Obstacle* obstacle : obstacles)
    {
        obstacle->update();
    }

    // Remove obstacles that have scrolled off screen
    removeOffScreenObstacles();

    // Spawn new obstacles
    ++framesSinceLastSpawn;

    if (framesSinceLastSpawn >= nextSpawnInterval)
    {
        spawnObstacle();
        framesSinceLastSpawn = 0;
        nextSpawnInterval = randomInterval();
    }

    // Increase score
    ++score;
}

void Game::spawnObstacle()
{
    Obstacle* newObstacle = new Obstacle(SCREEN_WIDTH, OBSTACLE_SPEED);
    obstacles.push_back(newObstacle);
}

void Game::removeOffScreenObstacles()
{
    // Iterate and delete obstacles that moved off the left side
    auto it = obstacles.begin();

    while (it != obstacles.end())
    {
        if ((*it)->isOffScreen())
        {
            delete *it;
            it = obstacles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool Game::checkCollisions() const
{
    int playerLeft = player.getX();
    int playerRight = playerLeft + player.getWidth();
    int playerTop = player.getY() - player.getHeight();
    int playerBottom = player.getY();

    for (const Obstacle* obstacle : obstacles)
    {
        int obstLeft = obstacle->getX();
        int obstRight = obstLeft + obstacle->getWidth();
        int obstTop = obstacle->getY() - obstacle->getHeight();
        int obstBottom = obstacle->getY();

        // AABB collision check
        bool horizontalOverlap = (playerLeft < obstRight) && (playerRight > obstLeft);
        bool verticalOverlap = (playerTop < obstBottom) && (playerBottom > obstTop);

        if (horizontalOverlap && verticalOverlap)
        {
            return true;
        }
    }

    return false;
}

void Game::cleanup()
{
    for (Obstacle* obstacle : obstacles)
    {
        delete obstacle;
    }

    obstacles.clear();
}

int Game::randomInterval() const
{
    int range = MAX_SPAWN_INTERVAL - MIN_SPAWN_INTERVAL;
    return MIN_SPAWN_INTERVAL + (std::rand() % (range + 1));
}

void Game::loadHighScore()
{
    std::ifstream file("highscore.txt");
    if (file.is_open())
    {
        file >> highScore;
        file.close();
    }
}

void Game::saveHighScore() const
{
    std::ofstream file("highscore.txt");
    if (file.is_open())
    {
        file << highScore;
        file.close();
    }
}
