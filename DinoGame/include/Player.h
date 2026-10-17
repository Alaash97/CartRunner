#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player();

    void jump();
    void update();

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool isOnGround() const;

private:
    static const float GRAVITY;
    static const float JUMP_VELOCITY;
    static const int GROUND_Y = 0;
    static const int PLAYER_X = 5;
    static const int PLAYER_WIDTH = 4;
    static const int PLAYER_HEIGHT = 3;

    float positionY;
    float velocityY;
    bool jumping;
};

#endif
