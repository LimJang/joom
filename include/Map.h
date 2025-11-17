#pragma once
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <memory>
#include "Chunk.h"
#include "ItemManager.h"

class MapGenerator; // Forward declaration

class Map {
public:
    Map();
    ~Map();

    void generateInitialChunk();
    void checkAndLoadChunks(float playerX, float playerY);

    bool isWallAt(float x, float y) const;
    int getWallType(int x, int y) const;
    
    // These methods will need to be adapted or re-thought for an infinite map
    int getWidth() const;
    int getHeight() const;

private:
    std::map<std::pair<int, int>, Chunk> chunks;
    std::unique_ptr<MapGenerator> mapGenerator;
};
