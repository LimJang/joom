#pragma once

#include "Pathfinder.h"
#include <vector>

class Player;
class Map;
class Renderer;
class LightSystem;
class AudioManager;

enum class MonsterState {
    IDLE,
    CHASING,
    ATTACKING
};

class Monster {
public:
    Monster(float x, float y);
    ~Monster();

    void update(Player* player, Map* map, Pathfinder* pathfinder, AudioManager* audioManager, float deltaTime);

    float getX() const { return x; }
    float getY() const { return y; }
    float getAnimationTime() const;
private:
    float x, y;
    float speed;
    MonsterState state;
    
    std::vector<Node*> path;
    float pathUpdateTimer; // 경로를 다시 계산하기 위한 타이머
    const float pathUpdateInterval = 0.5f; // 0.5초마다 경로 업데이트
    float animationTime;

    void followPath(Player* player, float deltaTime);
};

inline float Monster::getAnimationTime() const {
    return animationTime;
}
