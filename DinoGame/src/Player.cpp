#include "Player.h"

const float Player::GRAVITY = 0.8f;
const float Player::JUMP_VELOCITY = -5.0f;

Player::Player()
    : positionY(GROUND_Y),
      velocityY(0.0f),
      jumping(false)
{
}

void Player::jump()
{
    if (!jumping)
    {
        velocityY = JUMP_VELOCITY;
        jumping = true;
    }
}

void Player::update()
{
    if (jumping)
    {
        velocityY += GRAVITY;
        positionY += velocityY;

        if (positionY >= GROUND_Y)
        {
            positionY = GROUND_Y;
            velocityY = 0.0f;
            jumping = false;
        }
    }
}

int Player::getX() const
{
    return PLAYER_X;
}

int Player::getY() const
{
    return static_cast<int>(positionY);
}

int Player::getWidth() const
{
    return PLAYER_WIDTH;
}

int Player::getHeight() const
{
    return PLAYER_HEIGHT;
}

bool Player::isOnGround() const
{
    return !jumping;
}
