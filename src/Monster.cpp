#include "Monster.h"
#include "Player.h"
#include "Map.h"
#include "Renderer.h"
#include "AudioManager.h"
#include <iostream>

Monster::Monster(float x, float y)
    : x(x), y(y), speed(1.8f), state(MonsterState::IDLE), pathUpdateTimer(0.0f), animationTime(0.0f) {}

Monster::~Monster() {
    // 경로 노드 메모리 해제
    for (Node* node : path) {
        delete node;
    }
    path.clear();
}

void Monster::update(Player* player, Map* map, Pathfinder* pathfinder, AudioManager* audioManager, float deltaTime) {
    pathUpdateTimer += deltaTime;
    animationTime += deltaTime;

    float playerX = player->getX();
    float playerY = player->getY();
    float distanceToPlayer = std::sqrt(std::pow(x - playerX, 2) + std::pow(y - playerY, 2));

    // 사운드 업데이트
    if (audioManager) {
        // 10.0 유닛 거리부터 소리가 들리기 시작
        audioManager->updatePositionalSound("monster", distanceToPlayer, 10.0f);
    }

    // 플레이어와의 거리에 따라 상태 변경
    if (distanceToPlayer < 10.0f) { // 10 유닛 이내로 들어오면 추적 시작
        state = MonsterState::CHASING;
    } else {
        state = MonsterState::IDLE;
        // 경로 초기화
        for (Node* node : path) { delete node; }
        path.clear();
    }

    if (state == MonsterState::CHASING) {
        // 플레이어와 거리가 어느 정도 있을 때만 경로 탐색 실행
        if (distanceToPlayer > 1.0f && pathUpdateTimer >= pathUpdateInterval) {
            pathUpdateTimer = 0.0f;

            for (Node* node : path) { delete node; }
            path.clear();

            path = pathfinder->findPath(static_cast<int>(x), static_cast<int>(y), static_cast<int>(playerX), static_cast<int>(playerY), map);
        }
        
        // 경로를 따라가거나, 경로가 없으면(가까우면) 직접 플레이어를 추격
        followPath(player, deltaTime);
    }
}

void Monster::followPath(Player* player, float deltaTime) {
    float targetX, targetY;

    // 경로가 있고, 1개 이상의 노드가 남아있으면 경로를 따라감
    if (path.size() > 1) {
        Node* nextNode = path[1];
        targetX = nextNode->x + 0.5f; // 타일 중앙으로 이동
        targetY = nextNode->y + 0.5f;
    } else {
        // 경로가 없거나 마지막 노드에 도달하면, 플레이어를 직접 추격
        targetX = player->getX();
        targetY = player->getY();
    }

    float dx = targetX - x;
    float dy = targetY - y;
    float distanceToTarget = std::sqrt(dx * dx + dy * dy);

    // 목표에 거의 도달한 경우 (경로 추적 중에만 해당)
    if (path.size() > 1 && distanceToTarget < 0.1f) {
        delete path.front();
        path.erase(path.begin());
        return;
    }

    // 이동 (목표가 플레이어일 경우 멈추지 않음)
    if (distanceToTarget > 0.01f) {
        float moveX = (dx / distanceToTarget) * speed * deltaTime;
        float moveY = (dy / distanceToTarget) * speed * deltaTime;

        x += moveX;
        y += moveY;
    }
}

void Monster::render(Renderer* renderer, Player* player, LightSystem* lightSystem) const {
    // 렌더러에 몬스터를 그리는 기능은 Renderer 클래스에 구현해야 함
    // 여기서는 Renderer의 해당 함수를 호출만 함
    renderer->renderMonster(this, player, lightSystem);
}
