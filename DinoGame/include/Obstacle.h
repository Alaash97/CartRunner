#ifndef OBSTACLE_H
#define OBSTACLE_H

class Obstacle
{
public:
    Obstacle(int startX, float speed);

    void update();

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool isOffScreen() const;

private:
    static constexpr int OBSTACLE_WIDTH = 3;
    static constexpr int OBSTACLE_HEIGHT = 2;

    float positionX;
    float speed;
};

#endif
