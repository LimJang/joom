#pragma once

#include <vector>

const int CHUNK_SIZE = 16;

struct Chunk {
    std::vector<std::vector<int>> tiles;

    Chunk() : tiles(CHUNK_SIZE, std::vector<int>(CHUNK_SIZE, 1)) {} // Default to all walls
};
