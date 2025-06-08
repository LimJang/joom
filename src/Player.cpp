#include "Player.h"
#include "Map.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player::Player(float startX, float startY, float startAngle)
    : x(startX), y(startY), angle(startAngle), speed(3.0f), rotSpeed(2.0f) {
}

void Player::moveForward(float deltaTime, Map* map) {
    float newX = x + cos(angle) * speed * deltaTime;
    float newY = y + sin(angle) * speed * deltaTime;
    
    if (canMoveTo(newX, newY, map)) {
        x = newX;
        y = newY;
    }
}

void Player::moveBackward(float deltaTime, Map* map) {
    float newX = x - cos(angle) * speed * deltaTime;
    float newY = y - sin(angle) * speed * deltaTime;
    
    if (canMoveTo(newX, newY, map)) {
        x = newX;
        y = newY;
    }
}

void Player::strafeLeft(float deltaTime, Map* map) {
    float newX = x + cos(angle - M_PI/2) * speed * deltaTime;
    float newY = y + sin(angle - M_PI/2) * speed * deltaTime;
    
    if (canMoveTo(newX, newY, map)) {
        x = newX;
        y = newY;
    }
}

void Player::strafeRight(float deltaTime, Map* map) {
    float newX = x + cos(angle + M_PI/2) * speed * deltaTime;
    float newY = y + sin(angle + M_PI/2) * speed * deltaTime;
    
    if (canMoveTo(newX, newY, map)) {
        x = newX;
        y = newY;
    }
}

void Player::rotateLeft(float deltaTime) {
    angle -= rotSpeed * deltaTime;
    if (angle < 0) angle += 2 * M_PI;
}

void Player::rotateRight(float deltaTime) {
    angle += rotSpeed * deltaTime;
    if (angle >= 2 * M_PI) angle -= 2 * M_PI;
}

void Player::setAngle(float newAngle) {
    angle = newAngle;
}

bool Player::canMoveTo(float newX, float newY, Map* map) {
    // 경계 체크
    if (newX < 0.2f || newX >= map->getWidth() - 0.2f ||
        newY < 0.2f || newY >= map->getHeight() - 0.2f) {
        return false;
    }
    
    // 벽 충돌 체크
    return !map->isWallAt(newX, newY);
}
