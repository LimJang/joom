#include "Map.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

Map::Map(const std::string& mapDir) : width(16), height(16), tileSize(64), mapDirectory(mapDir), currentLevel(1) {
    createMapDirectory();
    createDefaultMapFiles();
    loadLevel(1);
}

Map::~Map() {
}

void Map::createMapDirectory() {
    namespace fs = std::filesystem;
    if (!fs::exists(mapDirectory)) {
        fs::create_directory(mapDirectory);
    }
}

void Map::createDefaultMapFiles() {
    // 레벨 1 - 튜토리얼 레벨 (빨간 키만 필요)
    std::ofstream level1(mapDirectory + "/level1.txt");
    level1 << "16 16\n";
    level1 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level1 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 2 2 2 0 0 0 0 3 3 3 0 0 1\n";
    level1 << "1 0 0 2 0 0 0 0 0 0 0 0 3 0 0 1\n";
    level1 << "1 0 0 2 0 0 0 0 0 0 0 0 3 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 2 2 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 2 2 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 3 3 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 3 0 0 0 0 0 0 0 0 2 0 0 1\n";
    level1 << "1 0 0 3 0 0 0 0 0 0 0 0 2 0 0 1\n";
    level1 << "1 0 0 3 3 3 0 0 0 0 2 2 2 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level1 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 9 1\n";  // 9 = 출구
    level1 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level1 << "ITEMS\n";
    level1 << "3.5 2.5 1\n";  // 빨간 키 - 플레이어 바로 오른쪽
    level1 << "2.5 3.5 4\n";   // 체력팩 - 플레이어 바로 아래
    level1.close();
    
    // 레벨 2 - 중급 레벨 (빨간키, 파란키 필요)
    std::ofstream level2(mapDirectory + "/level2.txt");
    level2 << "16 16\n";
    level2 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level2 << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n";
    level2 << "1 0 2 2 0 1 0 3 3 0 1 0 2 2 0 1\n";
    level2 << "1 0 2 2 0 0 0 3 3 0 0 0 2 2 0 1\n";
    level2 << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n";
    level2 << "1 1 0 1 1 1 1 1 1 1 1 1 1 0 1 1\n";
    level2 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level2 << "1 0 3 3 3 0 2 2 2 2 0 3 3 3 0 1\n";
    level2 << "1 0 3 3 3 0 2 2 2 2 0 3 3 3 0 1\n";
    level2 << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1\n";
    level2 << "1 1 0 1 1 1 1 1 1 1 1 1 1 0 1 1\n";
    level2 << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n";
    level2 << "1 0 2 2 0 1 0 3 3 0 1 0 2 2 0 1\n";
    level2 << "1 0 2 2 0 0 0 3 3 0 0 0 2 2 0 1\n";
    level2 << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 9 1\n";  // 9 = 출구
    level2 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level2 << "ITEMS\n";
    level2 << "7.5 6.5 1\n";   // 빨간 키 - 시작점 왼쪽
    level2 << "8.5 6.5 2\n";  // 파란 키 - 시작점 오른쪽
    level2 << "8.0 7.5 4\n";   // 체력팩 - 시작점 아래
    level2 << "8.0 5.5 5\n";   // 탄약팩 - 시작점 위
    level2.close();
    
    // 레벨 3 - 고급 레벨 (모든 키 필요)
    std::ofstream level3(mapDirectory + "/level3.txt");
    level3 << "16 16\n";
    level3 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level3 << "1 0 0 0 2 0 0 0 0 0 0 2 0 0 0 1\n";
    level3 << "1 0 3 0 2 0 1 1 1 1 0 2 0 3 0 1\n";
    level3 << "1 0 3 0 0 0 1 0 0 1 0 0 0 3 0 1\n";
    level3 << "1 2 0 0 1 0 1 0 0 1 0 1 0 0 2 1\n";
    level3 << "1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 1\n";
    level3 << "1 0 1 0 1 1 2 3 3 2 1 1 0 1 0 1\n";
    level3 << "1 0 1 0 0 3 0 0 0 0 3 0 0 1 0 1\n";
    level3 << "1 0 1 0 0 3 0 0 0 0 3 0 0 1 0 1\n";
    level3 << "1 0 1 0 1 1 2 3 3 2 1 1 0 1 0 1\n";
    level3 << "1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 1\n";
    level3 << "1 2 0 0 1 0 1 0 0 1 0 1 0 0 2 1\n";
    level3 << "1 0 3 0 0 0 1 0 0 1 0 0 0 3 0 1\n";
    level3 << "1 0 3 0 2 0 1 1 1 1 0 2 0 3 0 1\n";
    level3 << "1 0 0 0 2 0 0 0 0 0 0 2 0 0 9 1\n";  // 9 = 출구
    level3 << "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n";
    level3 << "ITEMS\n";
    level3 << "7.5 10.5 1\n";   // 빨간 키 - 시작점 왼쪽
    level3 << "8.5 10.5 2\n";  // 파란 키 - 시작점 오른쪽
    level3 << "9.5 10.5 3\n";   // 노란 키 - 시작점 오른쪽 더
    level3 << "6.5 10.5 4\n";   // 체력팩 - 시작점 왼쪽 더
    level3 << "5.5 10.5 5\n";  // 탄약팩 - 시작점 왼쪽 더더
    level3.close();
}

bool Map::loadMapFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // 맵 크기 읽기
    file >> width >> height;
    
    // 맵 데이터 초기화
    mapData.clear();
    mapData.resize(height, std::vector<int>(width, 0));
    
    // 맵 데이터 읽기
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            file >> mapData[y][x];
        }
    }
    
    file.close();
    return true;
}

void Map::loadLevel(int levelNumber) {
    currentLevel = levelNumber;
    std::string filename = mapDirectory + "/level" + std::to_string(levelNumber) + ".txt";
    
    if (!loadMapFromFile(filename)) {
        std::cerr << "Failed to load level " << levelNumber << ", using default map" << std::endl;
        loadDefaultMap();
    }
}

void Map::setupItemsForLevel(ItemManager* itemManager) {
    if (!itemManager) return;
    
    itemManager->clearItems();
    
    std::string filename = mapDirectory + "/level" + std::to_string(currentLevel) + ".txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "⚠️  Failed to open level file: " << filename << std::endl;
        return;
    }
    
    std::cout << "📁 Loading items for Level " << currentLevel << " from: " << filename << std::endl;
    
    std::string line;
    bool inItemsSection = false;
    int itemCount = 0;
    
    // 파일에서 ITEMS 섹션 찾기
    while (std::getline(file, line)) {
        if (line == "ITEMS") {
            inItemsSection = true;
            std::cout << "📎 Found ITEMS section" << std::endl;
            continue;
        }
        
        if (inItemsSection && !line.empty()) {
            std::istringstream iss(line);
            float x, y;
            int typeInt;
            
            if (iss >> x >> y >> typeInt) {
                ItemType type = static_cast<ItemType>(typeInt);

                // Filter out keys and health packs
                if (type == ItemType::KEY_RED || type == ItemType::KEY_BLUE || type == ItemType::KEY_YELLOW || type == ItemType::HEALTH_PACK) {
                    continue; // Skip adding these items
                }
                
                // 키 아이템의 경우 이미 수집했는지 확인
                bool shouldAdd = true;
                if (type == ItemType::KEY_RED && itemManager->hasKey(ItemType::KEY_RED)) {
                    shouldAdd = false;
                    std::cout << "🔴 빨간키는 이미 수집했으므로 생성하지 않음" << std::endl;
                } else if (type == ItemType::KEY_BLUE && itemManager->hasKey(ItemType::KEY_BLUE)) {
                    shouldAdd = false;
                    std::cout << "🔵 파란키는 이미 수집했으므로 생성하지 않음" << std::endl;
                } else if (type == ItemType::KEY_YELLOW && itemManager->hasKey(ItemType::KEY_YELLOW)) {
                    shouldAdd = false;
                    std::cout << "🟡 노란키는 이미 수집했으므로 생성하지 않음" << std::endl;
                }
                
                if (shouldAdd) {
                    itemManager->addItem(x, y, type);
                    itemCount++;
                    
                    // 아이템 생성 로그
                    if (type == ItemType::KEY_RED) {
                        std::cout << "🔴 빨간키 생성: (" << x << ", " << y << ")" << std::endl;
                    } else if (type == ItemType::KEY_BLUE) {
                        std::cout << "🔵 파란키 생성: (" << x << ", " << y << ")" << std::endl;
                    } else if (type == ItemType::KEY_YELLOW) {
                        std::cout << "🟡 노란키 생성: (" << x << ", " << y << ")" << std::endl;
                    } else if (type == ItemType::HEALTH_PACK) {
                        std::cout << "💚 체력팩 생성: (" << x << ", " << y << ")" << std::endl;
                    } else if (type == ItemType::AMMO_PACK) {
                        std::cout << "🟡 탄약팩 생성: (" << x << ", " << y << ")" << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << "✅ Total " << itemCount << " items loaded for Level " << currentLevel << std::endl;
    file.close();
}

void Map::loadDefaultMap() {
    width = 16;
    height = 16;
    mapData = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,2,2,2,0,0,0,0,3,3,3,0,0,1},
        {1,0,0,2,0,0,0,0,0,0,0,0,3,0,0,1},
        {1,0,0,2,0,0,0,0,0,0,0,0,3,0,0,1},
        {1,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,3,3,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,3,0,0,0,0,0,0,0,0,2,0,0,1},
        {1,0,0,3,0,0,0,0,0,0,0,0,2,0,0,1},
        {1,0,0,3,3,3,0,0,0,0,2,2,2,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,9,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
}

void Map::loadMap() {
    loadLevel(currentLevel);
}

bool Map::isWall(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return true;
    }
    int tile = mapData[y][x];
    return tile != 0 && tile != 9; // 0 = 빈공간, 9 = 출구
}

bool Map::isWallAt(float x, float y) const {
    return isWall(static_cast<int>(x), static_cast<int>(y));
}

bool Map::isExitTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    return mapData[y][x] == 9;
}

bool Map::isExitAt(float x, float y) const {
    return isExitTile(static_cast<int>(x), static_cast<int>(y));
}

int Map::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1;
    }
    return mapData[y][x];
}

int Map::getWallType(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1;
    }
    int tile = mapData[y][x];
    return (tile == 0 || tile == 9) ? 0 : tile; // 출구도 벽이 아님
}

bool Map::canAdvanceToNextLevel(const ItemManager* itemManager) const {
    return true; // Keys are no longer required
}

void Map::advanceToNextLevel(ItemManager* itemManager) {
    currentLevel++;
    
    // 다음 레벨 로드 시도
    std::string nextLevelFile = mapDirectory + "/level" + std::to_string(currentLevel) + ".txt";
    std::ifstream testFile(nextLevelFile);
    
    if (testFile.is_open()) {
        testFile.close();
        
        // 순서 수정: resetForNewLevel을 먼저 호출한 다음 아이템 설정
        if (itemManager) {
            itemManager->resetForNewLevel();  // 아이템 목록만 클리어 (키는 유지)
        }
        
        loadLevel(currentLevel);           // 맵 로드
        setupItemsForLevel(itemManager);   // 아이템 설정
        
        std::cout << "🎆 Level " << currentLevel << " loaded successfully!" << std::endl;
    } else {
        // 마지막 레벨인 경우
        currentLevel--;
        std::cout << "Congratulations! You've completed all levels!" << std::endl;
    }
}
