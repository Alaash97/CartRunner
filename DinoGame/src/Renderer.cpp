#include "Renderer.h"
#include "Player.h"
#include "Obstacle.h"

#include <iostream>
#include <sstream>

// ANSI Color Codes
const char* COLOR_RESET   = "\033[0m";
const char* COLOR_DINO    = "\033[33m";    // Yellow
const char* COLOR_CACTUS  = "\033[32m";    // Green
const char* COLOR_GROUND  = "\033[38;5;130m"; // Brown/Orange
const char* COLOR_SCORE   = "\033[36m";    // Cyan
const char* COLOR_CLOUD   = "\033[37m";    // White
const char* COLOR_TITLE   = "\033[1;35m";  // Magenta Bold

Renderer::Renderer()
    : needsFullClear(true)
{
    // A blinking caret is distracting during animation. It is restored by the
    // destructor so the terminal remains usable after the game exits.
    std::cout << "\033[?25l" << std::flush;
}

Renderer::~Renderer()
{
    std::cout << COLOR_RESET << "\033[?25h" << std::flush;
}

void Renderer::clearScreen() const
{
    std::cout << "\033[2J\033[1;1H";
}

int Renderer::getIndex(int row, int col) const
{
    return row * SCREEN_WIDTH + col;
}

void Renderer::setCharAt(std::vector<Cell>& buffer, int row, int col, char ch, const char* colorCode) const
{
    if (row >= 0 && row < SCREEN_HEIGHT && col >= 0 && col < SCREEN_WIDTH)
    {
        int index = getIndex(row, col);
        buffer[index].ch = ch;
        buffer[index].colorCode = colorCode;
    }
}

void Renderer::drawClouds(std::vector<Cell>& buffer, int score) const
{
    // Slowly moving clouds
    int cloudOffset1 = (score / 4) % SCREEN_WIDTH;
    int cloudOffset2 = (score / 6) % SCREEN_WIDTH;

    int c1_x = SCREEN_WIDTH - cloudOffset1;
    int c2_x = SCREEN_WIDTH - cloudOffset2 + 30; // Spaced out

    // Cloud 1
    setCharAt(buffer, 2, c1_x, '(', COLOR_CLOUD);
    setCharAt(buffer, 2, c1_x + 1, '~', COLOR_CLOUD);
    setCharAt(buffer, 2, c1_x + 2, '~', COLOR_CLOUD);
    setCharAt(buffer, 2, c1_x + 3, ')', COLOR_CLOUD);

    // Cloud 2
    if (c2_x < SCREEN_WIDTH) {
        setCharAt(buffer, 4, c2_x, '(', COLOR_CLOUD);
        setCharAt(buffer, 4, c2_x + 1, '~', COLOR_CLOUD);
        setCharAt(buffer, 4, c2_x + 2, ')', COLOR_CLOUD);
    } else if (c2_x - SCREEN_WIDTH < SCREEN_WIDTH) {
        setCharAt(buffer, 4, c2_x - SCREEN_WIDTH, '(', COLOR_CLOUD);
        setCharAt(buffer, 4, c2_x - SCREEN_WIDTH + 1, '~', COLOR_CLOUD);
        setCharAt(buffer, 4, c2_x - SCREEN_WIDTH + 2, ')', COLOR_CLOUD);
    }
}

void Renderer::drawGround(std::vector<Cell>& buffer, int score) const
{
    // Scrolling texture pattern
    std::string pattern = "___...---,,,";
    
    for (int col = 0; col < SCREEN_WIDTH; ++col)
    {
        int patternIndex = (col + score) % static_cast<int>(pattern.size());
        setCharAt(buffer, GROUND_ROW, col, pattern[patternIndex], COLOR_GROUND);
    }
}

void Renderer::drawPlayer(std::vector<Cell>& buffer, const Player& player) const
{
    // Player Y is 0 at ground, negative means up in the air
    // Convert to screen row: ground row - height offset - player Y offset
    int baseRow = GROUND_ROW - 1;
    int jumpOffset = -player.getY();
    int topRow = baseRow - 2 + (-jumpOffset);

    int col = player.getX();

    if (topRow >= 0 && topRow < SCREEN_HEIGHT)
    {
        setCharAt(buffer, topRow, col,     '.', COLOR_DINO);
        setCharAt(buffer, topRow, col + 1, '^', COLOR_DINO);
        setCharAt(buffer, topRow, col + 2, '^', COLOR_DINO);
        setCharAt(buffer, topRow, col + 3, '.', COLOR_DINO);
    }
    if (topRow + 1 >= 0 && topRow + 1 < SCREEN_HEIGHT)
    {
        setCharAt(buffer, topRow + 1, col,     '|', COLOR_DINO);
        setCharAt(buffer, topRow + 1, col + 1, '#', COLOR_DINO);
        setCharAt(buffer, topRow + 1, col + 2, '#', COLOR_DINO);
        setCharAt(buffer, topRow + 1, col + 3, '>', COLOR_DINO);
    }
    if (topRow + 2 >= 0 && topRow + 2 < SCREEN_HEIGHT)
    {
        setCharAt(buffer, topRow + 2, col,     'd', COLOR_DINO);
        setCharAt(buffer, topRow + 2, col + 1, ' ', COLOR_DINO);
        setCharAt(buffer, topRow + 2, col + 2, ' ', COLOR_DINO);
        setCharAt(buffer, topRow + 2, col + 3, 'd', COLOR_DINO);
    }
}

void Renderer::drawObstacles(std::vector<Cell>& buffer,
                              const std::vector<Obstacle*>& obstacles) const
{
    for (const Obstacle* obstacle : obstacles)
    {
        int col = obstacle->getX();
        int topRow = GROUND_ROW - obstacle->getHeight();

        for (int row = 0; row < obstacle->getHeight(); ++row)
        {
            int drawRow = topRow + row;
            setCharAt(buffer, drawRow, col,     '|', COLOR_CACTUS);
            setCharAt(buffer, drawRow, col + 1, '+', COLOR_CACTUS);
            setCharAt(buffer, drawRow, col + 2, '|', COLOR_CACTUS);
        }
    }
}

void Renderer::drawScore(std::vector<Cell>& buffer, int score, int highScore) const
{
    std::string scoreText = "HI: " + std::to_string(highScore) + "  SCORE: " + std::to_string(score);

    int startCol = SCREEN_WIDTH - static_cast<int>(scoreText.size()) - 1;

    for (int i = 0; i < static_cast<int>(scoreText.size()); ++i)
    {
        setCharAt(buffer, 0, startCol + i, scoreText[i], COLOR_SCORE);
    }
}

void Renderer::drawFrame(const Player& player,
                          const std::vector<Obstacle*>& obstacles,
                          int score, int highScore) const
{
    // Initialize buffer with empty cells and reset color
    std::vector<Cell> buffer(SCREEN_HEIGHT * SCREEN_WIDTH, {' ', COLOR_RESET});

    drawClouds(buffer, score);
    drawGround(buffer, score);
    drawPlayer(buffer, player);
    drawObstacles(buffer, obstacles);
    drawScore(buffer, score, highScore);

    // Build the final string with colors
    std::ostringstream out;
    const char* currentColor = COLOR_RESET;
    
    for (int row = 0; row < SCREEN_HEIGHT; ++row)
    {
        for (int col = 0; col < SCREEN_WIDTH; ++col)
        {
            const Cell& cell = buffer[getIndex(row, col)];
            
            // Only output color code if it changes
            if (cell.colorCode != currentColor)
            {
                out << cell.colorCode;
                currentColor = cell.colorCode;
            }
            out << cell.ch;
        }
        // Reset color before newline just in case
        if (currentColor != COLOR_RESET) {
            out << COLOR_RESET;
            currentColor = COLOR_RESET;
        }
        // Avoid a final newline, which can scroll a short console window and
        // make otherwise stable animation appear to jump.
        if (row + 1 < SCREEN_HEIGHT)
        {
            out << '\n';
        }
    }

    // Do not erase the display between animation frames. Erasing exposes a
    // blank console before the new frame arrives and is the main cause of
    // flicker. Cursor movement and frame data are emitted in one write so the
    // previous frame stays visible until it is overwritten.
    const char* cursorCommand = needsFullClear ? "\033[2J\033[H" : "\033[H";
    needsFullClear = false;
    std::cout << cursorCommand << out.str() << std::flush;
}

void Renderer::drawStartScreen() const
{
    clearScreen();

    std::cout << "\n\n";
    std::cout << "\033[1;35m" << "    =============================================\n";
    std::cout << "                    CART RUNNER\n";
    std::cout << "    =============================================" << "\033[0m" << "\n";
    std::cout << "\n";
    std::cout << "\033[33m" << "      .^^.\n";
    std::cout << "      |##>\n";
    std::cout << "      d  d\n" << "\033[0m";
    std::cout << "\n";
    std::cout << "    Controls:\n";
    std::cout << "      SPACE  -  Jump\n";
    std::cout << "      Q      -  Quit\n";
    std::cout << "\n";
    std::cout << "    Avoid the cacti and survive as long as you can!\n";
    std::cout << "\n";
    std::cout << "    Press SPACE to start...\n";
    std::cout << std::flush;
    needsFullClear = true;
}

void Renderer::drawGameOver(int score, int highScore) const
{
    clearScreen();

    std::cout << "\n\n\n";
    std::cout << "\033[1;31m" << "    =============================================\n";
    std::cout << "                     GAME OVER\n";
    std::cout << "    =============================================" << "\033[0m" << "\n";
    std::cout << "\n";
    std::cout << "      Score: \033[36m" << score << "\033[0m  |  High Score: \033[36m" << highScore << "\033[0m\n";
    std::cout << "\n";
    std::cout << "    Press SPACE to play again or Q to quit.\n";
    std::cout << std::flush;
    needsFullClear = true;
}
