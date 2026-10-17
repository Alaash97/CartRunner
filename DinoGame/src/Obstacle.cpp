#include "Obstacle.h"

Obstacle::Obstacle(int startX, float speed)
    : positionX(static_cast<float>(startX)),
      speed(speed)
{
}

void Obstacle::update()
{
    positionX -= speed;
}

int Obstacle::getX() const
{
    return static_cast<int>(positionX);
}

int Obstacle::getY() const
{
    return 0;
}

int Obstacle::getWidth() const
{
    return OBSTACLE_WIDTH;
}

int Obstacle::getHeight() const
{
    return OBSTACLE_HEIGHT;
}

bool Obstacle::isOffScreen() const
{
    return positionX + OBSTACLE_WIDTH < 0;
}
