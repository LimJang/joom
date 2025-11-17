#include "Renderer.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(SDL_Renderer* sdlRenderer, int width, int height, TextureManager* texMgr, LightSystem* lights)
    : renderer(sdlRenderer), screenWidth(width), screenHeight(height), textureManager(texMgr), lightSystem(lights) {
    
    depthBuffer.resize(screenWidth);
    screenBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);

    // Profiling variables
    profilingTimer = SDL_GetTicks();
    wallTimeAccumulator = 0;
    floorTimeAccumulator = 0;
    frameCounterForProfile = 0;
}



Renderer::~Renderer() {

    if (screenBuffer) {

        SDL_DestroyTexture(screenBuffer);

    }

}

void Renderer::initializeTextures() {
    // 각 텍스처를 개별적으로 로딩 시도
    bool brick_loaded = textureManager->loadTexture("wall_brick", "wall_brick.png");
    bool stone_loaded = textureManager->loadTexture("wall_stone", "wall_stone.png");
    bool metal_loaded = textureManager->loadTexture("wall_metal", "wall_metal.png");
    bool floor_loaded = textureManager->loadTexture("floor_stone", "floor_stone.png");
    bool ceiling_loaded = textureManager->loadTexture("ceiling_metal", "ceiling_metal.png");

    // 실패한 텍스처만 프로시저럴로 생성
    if (!brick_loaded) {
        textureManager->createWallTexture("wall_brick", 64, 64, 0);
    }
    if (!stone_loaded) {
        textureManager->createWallTexture("wall_stone", 64, 64, 1);
    }
    if (!metal_loaded) {
        textureManager->createWallTexture("wall_metal", 64, 64, 2);
    }
    if (!floor_loaded) {
        textureManager->createFloorTexture("floor_stone", 64, 64);
    }
    if (!ceiling_loaded) {
        textureManager->createCeilingTexture("ceiling_metal", 64, 64);
    }
}

void Renderer::present() {
    SDL_RenderCopy(renderer, screenBuffer, NULL, NULL);

    // Profiling output
    if (SDL_GetTicks() - profilingTimer > 1000) {
        if (frameCounterForProfile > 0) {
            float avgWallTime = (float)wallTimeAccumulator / frameCounterForProfile;
            float avgFloorTime = (float)floorTimeAccumulator / frameCounterForProfile;
            std::cout << "Avg Frame Time -> Walls: " << avgWallTime << "ms, Floor/Ceiling: " << avgFloorTime << "ms" << std::endl;
        }
        profilingTimer = SDL_GetTicks();
        wallTimeAccumulator = 0;
        floorTimeAccumulator = 0;
        frameCounterForProfile = 0;
    }
}

void Renderer::render(Player* player, Map* map, const std::vector<Item>& items, const Monster* monster) {
    void* pixels;
    int pitch;
    SDL_LockTexture(screenBuffer, NULL, &pixels, &pitch);

    Uint32* pixelPtr = static_cast<Uint32*>(pixels);

    if (!lightSystem->isFlashlightEnabled()) {
        SDL_memset(pixelPtr, 0, screenHeight * pitch);
    } else {
        renderFloorAndCeiling(player, pixelPtr);
        renderWalls(player, map, pixelPtr);
        // renderSprites(player, items, monster, pixelPtr); // Sprites are disabled for now
    }

    SDL_UnlockTexture(screenBuffer);
}

Uint32 Renderer::applyLighting(Uint32 color, float lighting) const {
    lighting = std::clamp(lighting, 0.0f, 1.0f);
    Uint8 a = (color >> 24) & 0xFF;
    Uint8 r = (color >> 16) & 0xFF;
    Uint8 g = (color >> 8) & 0xFF;
    Uint8 b = color & 0xFF;

    r = static_cast<Uint8>(r * lighting);
    g = static_cast<Uint8>(g * lighting);
    b = static_cast<Uint8>(b * lighting);

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void Renderer::renderFloorAndCeiling(Player* player, Uint32* pixels) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    float fovRadians = degreesToRadians(FOV);

    float rayDirX0 = cos(playerAngle - fovRadians / 2);
    float rayDirY0 = sin(playerAngle - fovRadians / 2);
    float rayDirX1 = cos(playerAngle + fovRadians / 2);
    float rayDirY1 = sin(playerAngle + fovRadians / 2);

    int floorTexWidth, floorTexHeight, ceilingTexWidth, ceilingTexHeight;
    const std::vector<Uint32>* floorPixels = textureManager->getPixels("floor_stone", floorTexWidth, floorTexHeight);
    const std::vector<Uint32>* ceilingPixels = textureManager->getPixels("ceiling_metal", ceilingTexWidth, ceilingTexHeight);

    if (!floorPixels || !ceilingPixels) return;

    for (int y = screenHeight / 2; y < screenHeight; ++y) {
        float rowDistance = (0.5f * screenHeight) / (y - screenHeight / 2.0f);

        float floorX_step = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
        float floorY_step = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

        float floorX = playerX + rowDistance * rayDirX0;
        float floorY = playerY + rowDistance * rayDirY0;

        float lighting = lightSystem->getLightFromDistanceLUT(rowDistance) + lightSystem->getAmbientLight();

        for (int x = 0; x < screenWidth; ++x) {
            int texX_floor = static_cast<int>(floorX * floorTexWidth) & (floorTexWidth - 1);
            int texY_floor = static_cast<int>(floorY * floorTexHeight) & (floorTexHeight - 1);
            Uint32 floorColor = (*floorPixels)[texY_floor * floorTexWidth + texX_floor];

            int texX_ceil = static_cast<int>(floorX * ceilingTexWidth) & (ceilingTexWidth - 1);
            int texY_ceil = static_cast<int>(floorY * ceilingTexHeight) & (ceilingTexHeight - 1);
            Uint32 ceilingColor = (*ceilingPixels)[texY_ceil * ceilingTexWidth + texX_ceil];

            pixels[y * screenWidth + x] = applyLighting(floorColor, lighting);
            pixels[(screenHeight - y - 1) * screenWidth + x] = applyLighting(ceilingColor, lighting);

            floorX += floorX_step;
            floorY += floorY_step;
        }
    }
}

void Renderer::renderWalls(Player* player, Map* map, Uint32* pixels) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    float startAngle = playerAngle - degreesToRadians(FOV / 2);
    float angleIncrement = degreesToRadians(FOV) / screenWidth;

    std::fill(depthBuffer.begin(), depthBuffer.end(), 20.0f);

    // Pre-fetch texture data
    int brickW, brickH, stoneW, stoneH, metalW, metalH;
    const std::vector<Uint32>* brickPx = textureManager->getPixels("wall_brick", brickW, brickH);
    const std::vector<Uint32>* stonePx = textureManager->getPixels("wall_stone", stoneW, stoneH);
    const std::vector<Uint32>* metalPx = textureManager->getPixels("wall_metal", metalW, metalH);

    for (int x = 0; x < screenWidth; ++x) {
        float rayAngle = startAngle + x * angleIncrement;
        
        float hitX, hitY;
        int wallType = 0;
        float distance = 0;
        float step = 0.05f;
        float rayX = playerX, rayY = playerY;
        float rayDX = cos(rayAngle) * step, rayDY = sin(rayAngle) * step;

        while (distance < 20.0f) {
            rayX += rayDX;
            rayY += rayDY;
            distance += step;
            if (map->isWallAt(rayX, rayY)) {
                wallType = map->getWallType(static_cast<int>(rayX), static_cast<int>(rayY));
                hitX = rayX;
                hitY = rayY;
                break;
            }
        }

        if (wallType == 0) continue;

        float correctedDistance = distance * cos(rayAngle - playerAngle);
        depthBuffer[x] = correctedDistance;

        int wallHeight = static_cast<int>((screenHeight / correctedDistance) * 0.6f);
        int wallTop = std::max(0, (screenHeight - wallHeight) / 2);
        int wallBottom = std::min(screenHeight, (screenHeight + wallHeight) / 2);

        float wallX;
        if (std::abs(hitX - round(hitX)) < std::abs(hitY - round(hitY))) {
            wallX = hitY - floor(hitY);
        } else {
            wallX = hitX - floor(hitX);
        }

        const std::vector<Uint32>* texturePx = nullptr;
        int texWidth = 0, texHeight = 0;

        switch (wallType) {
            case 1: texturePx = brickPx; texWidth = brickW; texHeight = brickH; break;
            case 2: texturePx = stonePx; texWidth = stoneW; texHeight = stoneH; break;
            case 3: texturePx = metalPx; texWidth = metalW; texHeight = metalH; break;
            default: texturePx = brickPx; texWidth = brickW; texHeight = brickH; break;
        }

        if (!texturePx) continue;

        float lighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, hitX, hitY, correctedDistance);
        if (lighting < 0.05f) continue;

        int texX = static_cast<int>(wallX * texWidth) & (texWidth - 1);

        for (int y = wallTop; y < wallBottom; ++y) {
            float texY_float = (float)(y - wallTop) / (float)wallHeight;
            int texY = static_cast<int>(texY_float * texHeight) & (texHeight - 1);
            Uint32 color = (*texturePx)[texY * texWidth + texX];
            pixels[y * screenWidth + x] = applyLighting(color, lighting);
        }
    }
}

void Renderer::renderSprites(Player* player, const std::vector<Item>& items, const Monster* monster, Uint32* pixels) {
    // Sprite rendering logic will be implemented here in the future.
}

float Renderer::degreesToRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

// 미니맵 렌더링은 이전과 동일하게 유지 (직접 렌더러에 그림)
void Renderer::renderMiniMap(Player* player, Map* map) {
    const int miniMapSize = 150;
    const int miniMapX = screenWidth - miniMapSize - 10;
    const int miniMapY = 10;
    const int tileSize = miniMapSize / map->getWidth();
    
    SDL_Rect miniMapBg = {miniMapX - 2, miniMapY - 2, miniMapSize + 4, miniMapSize + 4};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &miniMapBg);
    
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            SDL_Rect tileRect = { miniMapX + x * tileSize, miniMapY + y * tileSize, tileSize, tileSize };
            int tileType = map->getWallType(x, y);
            
            if (tileType == 9) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            } else if (tileType != 0) {
                switch (tileType) {
                    case 1: SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); break;
                    case 2: SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255); break;
                    case 3: SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255); break;
                    default: SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); break;
                }
            } else {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            }
            SDL_RenderFillRect(renderer, &tileRect);
        }
    }
    
    int playerPixelX = miniMapX + static_cast<int>(player->getX() * tileSize);
    int playerPixelY = miniMapY + static_cast<int>(player->getY() * tileSize);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect playerRect = {playerPixelX - 2, playerPixelY - 2, 4, 4};
    SDL_RenderFillRect(renderer, &playerRect);
    
    float dirX = cos(player->getAngle()) * tileSize * 0.5f;
    float dirY = sin(player->getAngle()) * tileSize * 0.5f;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer, playerPixelX, playerPixelY, playerPixelX + static_cast<int>(dirX), playerPixelY + static_cast<int>(dirY));
}
