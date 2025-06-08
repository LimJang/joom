#pragma once
#include <cmath>

class Player {
private:
    float x, y;        // 플레이어 위치
    float angle;       // 플레이어가 바라보는 각도
    float speed;       // 이동 속도
    float rotSpeed;    // 회전 속도
    
public:
    Player(float startX, float startY, float startAngle);
    
    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    
    // Movement
    void moveForward(float deltaTime, class Map* map);
    void moveBackward(float deltaTime, class Map* map);
    void strafeLeft(float deltaTime, class Map* map);
    void strafeRight(float deltaTime, class Map* map);
    void rotateLeft(float deltaTime);
    void rotateRight(float deltaTime);
    void setAngle(float newAngle);
    
    // Collision check
    bool canMoveTo(float newX, float newY, class Map* map);
};
