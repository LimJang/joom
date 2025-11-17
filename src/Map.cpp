#include "Map.h"
#include "MapGenerator.h"
#include <cmath>
#include <iostream>
#include <random>

Map::Map() {
    // Initialize the map generator with a random seed
    std::random_device rd;
    unsigned int seed = rd();
    std::cout << "MapGenerator seed: " << seed << std::endl;
    mapGenerator = std::make_unique<MapGenerator>(seed);
}

Map::~Map() {
}

void Map::generateInitialChunk() {
    std::cout << "Generating initial chunk with Perlin noise..." << std::endl;
    // Generate the first chunk at (0,0)
    chunks[{0, 0}] = mapGenerator->generateChunk(0, 0);
}

void Map::checkAndLoadChunks(float playerX, float playerY) {
    int playerChunkX = floor(playerX / CHUNK_SIZE);
    int playerChunkY = floor(playerY / CHUNK_SIZE);
    const int loadRadius = 1; // Load a 3x3 grid of chunks around the player

    for (int y = playerChunkY - loadRadius; y <= playerChunkY + loadRadius; ++y) {
        for (int x = playerChunkX - loadRadius; x <= playerChunkX + loadRadius; ++x) {
            if (chunks.find({x, y}) == chunks.end()) {
                // Chunk is not loaded, so generate it
                std::cout << "Player is near unloaded area. Generating new chunk at (" << x << ", " << y << ")..." << std::endl;
                chunks[{x, y}] = mapGenerator->generateChunk(x, y);
            }
        }
    }
}

int Map::getWallType(int x, int y) const {
    // 1. Calculate chunk coordinates
    int chunkX = floor(static_cast<float>(x) / CHUNK_SIZE);
    int chunkY = floor(static_cast<float>(y) / CHUNK_SIZE);

    // 2. Find the chunk
    auto it = chunks.find({chunkX, chunkY});
    if (it == chunks.end()) {
        return 1; // If chunk is not loaded, treat as a solid wall
    }

    // 3. Calculate tile coordinates within the chunk
    int tileX = x % CHUNK_SIZE;
    int tileY = y % CHUNK_SIZE;
    if (tileX < 0) tileX += CHUNK_SIZE;
    if (tileY < 0) tileY += CHUNK_SIZE;

    // 4. Get the tile from the chunk
    if (tileY >= 0 && tileY < CHUNK_SIZE && tileX >= 0 && tileX < CHUNK_SIZE) {
        return it->second.tiles[tileY][tileX];
    }

    return 1; // Out of bounds within the chunk, treat as a wall
}

bool Map::isWallAt(float x, float y) const {
    int wallType = getWallType(static_cast<int>(floor(x)), static_cast<int>(floor(y)));
    return wallType != 0;
}

// These are now approximations, as the map is infinite.
// They are needed for some legacy functions (e.g., player boundary checks).
int Map::getWidth() const {
    return CHUNK_SIZE; // For now, just return the size of one chunk
}

int Map::getHeight() const {
    return CHUNK_SIZE; // For now, just return the size of one chunk
}
