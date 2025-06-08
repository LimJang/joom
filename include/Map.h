#pragma once
#include <vector>

class Map {
private:
    std::vector<std::vector<int>> mapData;
    int width, height;
    int tileSize;
    
public:
    Map();
    ~Map();
    
    void loadMap();
    bool isWall(int x, int y) const;
    bool isWallAt(float x, float y) const;
    int getWallType(int x, int y) const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTileSize() const { return tileSize; }
    
    // 2D 맵 렌더링용 (미니맵)
    int getTile(int x, int y) const;
    const std::vector<std::vector<int>>& getMapData() const { return mapData; }
};
