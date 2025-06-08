#include "Map.h"

Map::Map() : width(16), height(16), tileSize(64) {
    loadMap();
}

Map::~Map() {
}

void Map::loadMap() {
    // 다양한 벽 타입을 가진 테스트 맵 (16x16)
    // 0 = 빈 공간, 1 = 벽돌, 2 = 돌벽, 3 = 금속벽
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
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
}

bool Map::isWall(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return true;  // 맵 경계 밖은 벽으로 처리
    }
    return mapData[y][x] != 0;  // 0이 아닌 모든 값은 벽
}

bool Map::isWallAt(float x, float y) const {
    return isWall(static_cast<int>(x), static_cast<int>(y));
}

int Map::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1;  // 경계 밖은 벽
    }
    return mapData[y][x];
}

int Map::getWallType(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1;  // 경계 밖은 기본 벽돌
    }
    return mapData[y][x];
}
