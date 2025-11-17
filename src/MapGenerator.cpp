#include "MapGenerator.h"

MapGenerator::MapGenerator(unsigned int seed) : perlin(seed) {
}

Chunk MapGenerator::generateChunk(int chunkX, int chunkY) {
    Chunk newChunk;

    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            // Calculate global coordinates for the noise function
            double globalX = (double)(chunkX * CHUNK_SIZE + x);
            double globalY = (double)(chunkY * CHUNK_SIZE + y);

            // Get the 2D Perlin noise value. It's between -1.0 and 1.0.
            // We use octave noise for more detail.
            double noiseValue = perlin.octave2D_01(globalX * frequency, globalY * frequency, 4);

            // If the noise value is above the threshold, it's a wall.
            if (noiseValue > threshold) {
                newChunk.tiles[y][x] = 1; // Wall
            } else {
                newChunk.tiles[y][x] = 0; // Floor
            }
        }
    }

    return newChunk;
}
