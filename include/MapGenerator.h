#pragma once

#include "Chunk.h"
#include "PerlinNoise.hpp"

class MapGenerator {
public:
    MapGenerator(unsigned int seed);

    Chunk generateChunk(int chunkX, int chunkY);

private:
    siv::PerlinNoise perlin;
    double frequency = 0.05; // Controls the "zoom" level of the noise
    double threshold = 0.5;  // Determines wall vs. floor
};
