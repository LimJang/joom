#pragma once
#include <string>
#include <vector>

enum class ItemType {
    KEY_RED = 1,
    KEY_BLUE = 2,
    KEY_YELLOW = 3,
    HEALTH_PACK = 4,
    AMMO_PACK = 5,
    EXIT_PORTAL = 6
};

struct Item {
    float x, y;           // 아이템 위치
    ItemType type;        // 아이템 종류
    bool collected;       // 수집 여부
    float animationTime;  // 애니메이션용 시간
    
    Item(float posX, float posY, ItemType itemType) 
        : x(posX), y(posY), type(itemType), collected(false), animationTime(0.0f) {}
};

class ItemManager {
private:
    std::vector<Item> items;
    int redKeys, blueKeys, yellowKeys;
    int health, ammo;
    
public:
    ItemManager();
    ~ItemManager();
    
    // 아이템 관리
    void addItem(float x, float y, ItemType type);
    void clearItems();
    bool checkItemCollision(float playerX, float playerY, float radius = 0.3f);
    void update(float deltaTime);
    
    // 아이템 상태 조회
    const std::vector<Item>& getItems() const { return items; }
    int getKeyCount(ItemType keyType) const;
    bool hasKey(ItemType keyType) const;
    bool hasAllKeys() const;
    
    // 플레이어 상태
    int getHealth() const { return health; }
    int getAmmo() const { return ammo; }
    void setHealth(int h) { health = h; }
    void setAmmo(int a) { ammo = a; }
    
    // 아이템 수집
    void collectItem(size_t index);
    
    // 레벨 초기화
    void resetForNewLevel();
};
