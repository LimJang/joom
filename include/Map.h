#pragma once
#include <vector>
#include <string>
#include "ItemManager.h"

class Map {
public:
    Map(const std::string& mapDir);
    ~Map();

    void loadMap();
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTile(int x, int y) const;
    bool isWall(int x, int y) const;
    bool isWallAt(float x, float y) const;
    int getWallType(int x, int y) const;
    bool isExitAt(float x, float y) const;
    
    // 레벨 관리
    int getCurrentLevel() const { return currentLevel; }
    void advanceToNextLevel(ItemManager* itemManager);
    bool canAdvanceToNextLevel(const ItemManager* itemManager) const;
    void setupItemsForLevel(ItemManager* itemManager);

private:
    int width, height;
    int tileSize;
    std::vector<std::vector<int>> mapData;
    std::string mapDirectory;
    int currentLevel;

    void loadDefaultMap();
    void loadLevel(int levelNumber);
    bool loadMapFromFile(const std::string& filename);
    bool isExitTile(int x, int y) const;
    void createMapDirectory();
    void createDefaultMapFiles();
};
