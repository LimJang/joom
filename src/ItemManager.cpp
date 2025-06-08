#include "ItemManager.h"
#include <cmath>
#include <algorithm>

ItemManager::ItemManager() 
    : redKeys(0), blueKeys(0), yellowKeys(0), health(100), ammo(50) {
}

ItemManager::~ItemManager() {
}

void ItemManager::addItem(float x, float y, ItemType type) {
    items.emplace_back(x, y, type);
}

void ItemManager::clearItems() {
    items.clear();
}

bool ItemManager::checkItemCollision(float playerX, float playerY, float radius) {
    bool collected = false;
    
    for (size_t i = 0; i < items.size(); ++i) {
        Item& item = items[i];
        if (item.collected) continue;
        
        // 거리 계산
        float dx = item.x - playerX;
        float dy = item.y - playerY;
        float distance = sqrt(dx * dx + dy * dy);
        
        // 충돌 검사
        if (distance < radius) {
            collectItem(i);
            collected = true;
        }
    }
    
    return collected;
}

void ItemManager::update(float deltaTime) {
    // 아이템 애니메이션 업데이트
    for (Item& item : items) {
        if (!item.collected) {
            item.animationTime += deltaTime;
        }
    }
}

int ItemManager::getKeyCount(ItemType keyType) const {
    switch (keyType) {
        case ItemType::KEY_RED: return redKeys;
        case ItemType::KEY_BLUE: return blueKeys;
        case ItemType::KEY_YELLOW: return yellowKeys;
        default: return 0;
    }
}

bool ItemManager::hasKey(ItemType keyType) const {
    return getKeyCount(keyType) > 0;
}

bool ItemManager::hasAllKeys() const {
    return redKeys > 0 && blueKeys > 0 && yellowKeys > 0;
}

void ItemManager::collectItem(size_t index) {
    if (index >= items.size()) return;
    
    Item& item = items[index];
    if (item.collected) return;
    
    item.collected = true;
    
    // 아이템 효과 적용
    switch (item.type) {
        case ItemType::KEY_RED:
            redKeys++;
            break;
        case ItemType::KEY_BLUE:
            blueKeys++;
            break;
        case ItemType::KEY_YELLOW:
            yellowKeys++;
            break;
        case ItemType::HEALTH_PACK:
            health = std::min(100, health + 25);
            break;
        case ItemType::AMMO_PACK:
            ammo = std::min(100, ammo + 20);
            break;
        case ItemType::EXIT_PORTAL:
            // 게임에서 처리
            break;
    }
}

void ItemManager::resetForNewLevel() {
    clearItems();
    // 키는 레벨 간 지속되도록 유지
    // redKeys, blueKeys, yellowKeys는 그대로 유지
    // health와 ammo도 지속
}
