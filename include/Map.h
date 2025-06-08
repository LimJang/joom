#pragma once
#include <vector>
#include <string>
#include "ItemManager.h"

class Map {
private:
    std::vector<std::vector<int>> mapData;
    int width, height;
    int tileSize;
    int currentLevel;
    std::string mapDirectory;
    
public:
    Map();
    ~Map();
    
    // 맵 로딩
    void loadMap();
    bool loadMapFromFile(const std::string& filename);
    void loadLevel(int levelNumber);
    void setupItemsForLevel(ItemManager* itemManager);
    
    // 기존 기능
    bool isWall(int x, int y) const;
    bool isWallAt(float x, float y) const;
    int getWallType(int x, int y) const;
    
    // 접근자
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTileSize() const { return tileSize; }
    int getCurrentLevel() const { return currentLevel; }
    
    // 출구 체크
    bool isExitTile(int x, int y) const;
    bool isExitAt(float x, float y) const;
    
    // 2D 맵 렌더링용 (미니맵)
    int getTile(int x, int y) const;
    const std::vector<std::vector<int>>& getMapData() const { return mapData; }
    
    // 다음 레벨로 진행
    bool canAdvanceToNextLevel(const ItemManager* itemManager) const;
    void advanceToNextLevel(ItemManager* itemManager);
    
private:
    void loadDefaultMap();
    void createMapDirectory();
    void createDefaultMapFiles();
};
