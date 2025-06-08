#include "Renderer.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(SDL_Renderer* sdlRenderer, int width, int height, TextureManager* texMgr, LightSystem* lights)
    : renderer(sdlRenderer), screenWidth(width), screenHeight(height), textureManager(texMgr), lightSystem(lights) {
}

Renderer::~Renderer() {
}

void Renderer::initializeTextures() {
    // 각 텍스처를 개별적으로 로딩 시도
    bool brick_loaded = textureManager->loadTexture("wall_brick", "textures/wall_brick.png");
    bool stone_loaded = textureManager->loadTexture("wall_stone", "textures/wall_stone.png");
    bool metal_loaded = textureManager->loadTexture("wall_metal", "textures/wall_metal.png");
    
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
    
    textureManager->createFloorTexture("floor", 64, 64);
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);  // 어두운 회색 배경
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::render3D(Player* player, Map* map) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    
    // 손전등이 꺼져있으면 완전 암흑
    if (!lightSystem->isFlashlightEnabled()) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        return; // 아무것도 렌더링하지 않음
    }
    
    // 배경을 완전히 어둡게 설정
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 완전히 검은색
    SDL_RenderClear(renderer);
    
    // 바닥과 천장 렌더링 (최적화된 버전)
    renderFloorAndCeiling(player, map);
    
    // 레이캐스팅으로 벽 렌더링
    float startAngle = playerAngle - degreesToRadians(FOV / 2);
    float angleIncrement = degreesToRadians(FOV) / screenWidth;
    
    for (int x = 0; x < screenWidth; x++) {
        float rayAngle = startAngle + x * angleIncrement;
        
        // 레이 캐스팅
        int wallType = 0;
        float hitX, hitY;
        float distance = castRay(playerX, playerY, rayAngle, map, wallType, hitX, hitY);
        
        // 거리에 따른 벽 높이 계산
        float wallHeight = (screenHeight / distance) * 0.6f;
        
        // 화면 중앙에서 벽 그리기
        int wallTop = (screenHeight - wallHeight) / 2;
        int wallBottom = wallTop + wallHeight;
        
        // 텍스처 좌표 계산 (간단한 버전)
        float textureX = 0.5f; // 나중에 정확한 텍스처 좌표 계산
        
        // 조명 계산
        float lighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, hitX, hitY, distance);
        
        // 텍스처가 있고 충분히 밝으면 렌더링
        if (wallType > 0 && lighting > 0.15f) { // 임계값을 0.15로 더 올림
            drawTexturedVerticalLine(x, wallHeight, wallType, textureX, lighting);
        }
        // 어두우면 아무것도 그리지 않음 (완전 어둠)
    }
}

void Renderer::renderMiniMap(Player* player, Map* map) {
    const int miniMapSize = 150;
    const int miniMapX = screenWidth - miniMapSize - 10;
    const int miniMapY = 10;
    const int tileSize = miniMapSize / map->getWidth();
    
    // 미니맵 배경
    SDL_Rect miniMapBg = {miniMapX - 2, miniMapY - 2, miniMapSize + 4, miniMapSize + 4};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &miniMapBg);
    
    // 맵 타일들 그리기
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            SDL_Rect tileRect = {
                miniMapX + x * tileSize,
                miniMapY + y * tileSize,
                tileSize,
                tileSize
            };
            
            int tileType = map->getTile(x, y);
            
            if (tileType != 0) {  // 벽이면
                // 벽 타입에 따른 색상 구분
                switch (tileType) {
                    case 1: // 벽돌 - 갈색
                        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                        break;
                    case 2: // 돌 - 회색
                        SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
                        break;
                    case 3: // 금속 - 어두운 금속색
                        SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
                        break;
                    default: // 기본 벽 - 흰색
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        break;
                }
            } else {  // 빈 공간
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);    // 어두운 회색
            }
            
            SDL_RenderFillRect(renderer, &tileRect);
            
            // 경계선 그리기 (명확한 구분을 위해)
            if (tileType != 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);  // 어두운 경계선
                SDL_RenderDrawRect(renderer, &tileRect);
            }
        }
    }
    
    // 플레이어 위치 표시
    int playerPixelX = miniMapX + static_cast<int>(player->getX() * tileSize);
    int playerPixelY = miniMapY + static_cast<int>(player->getY() * tileSize);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // 빨간색
    SDL_Rect playerRect = {playerPixelX - 2, playerPixelY - 2, 4, 4};
    SDL_RenderFillRect(renderer, &playerRect);
    
    // 플레이어 시선 방향 표시
    float dirX = cos(player->getAngle()) * tileSize * 0.5f;
    float dirY = sin(player->getAngle()) * tileSize * 0.5f;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // 노란색
    SDL_RenderDrawLine(renderer, playerPixelX, playerPixelY,
                       playerPixelX + static_cast<int>(dirX),
                       playerPixelY + static_cast<int>(dirY));
}

float Renderer::castRay(float startX, float startY, float angle, Map* map, int& wallType, float& hitX, float& hitY) {
    float rayX = startX;
    float rayY = startY;
    float deltaX = cos(angle) * RAY_STEP;
    float deltaY = sin(angle) * RAY_STEP;
    float distance = 0;
    
    wallType = 0; // 초기값
    
    while (distance < MAX_DISTANCE) {
        rayX += deltaX;
        rayY += deltaY;
        distance += RAY_STEP;
        
        if (map->isWallAt(rayX, rayY)) {
            wallType = map->getWallType(static_cast<int>(rayX), static_cast<int>(rayY));
            hitX = rayX;
            hitY = rayY;
            return distance;
        }
    }
    
    // 벽에 맞지 않았을 때의 기본값
    hitX = rayX;
    hitY = rayY;
    return MAX_DISTANCE;
}

void Renderer::drawTexturedVerticalLine(int x, int wallHeight, int wallType, float textureX, float lighting) {
    int wallTop = (screenHeight - wallHeight) / 2;
    int wallBottom = wallTop + wallHeight;
    
    // 텍스처 이름 결정
    std::string textureName;
    switch (wallType) {
        case 1: textureName = "wall_brick"; break;
        case 2: textureName = "wall_stone"; break;
        case 3: textureName = "wall_metal"; break;
        default: textureName = "wall_brick"; break;
    }
    
    // SDL 텍스처 가져오기
    SDL_Texture* texture = textureManager->getTexture(textureName);
    
    if (texture) {
        // 실제 텍스처 렌더링
        SDL_Rect srcRect = {0, 0, 64, 64};  // 전체 텍스처 사용
        SDL_Rect dstRect = {x, wallTop, 1, static_cast<int>(wallHeight)};
        
        // 조명 효과 적용 (텍스처 색상 조절)
        Uint8 lightValue = static_cast<Uint8>(255 * lighting);
        SDL_SetTextureColorMod(texture, lightValue, lightValue, lightValue);
        
        SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
        
        // 색상 모드 초기화
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    } else {
        // 텍스처가 없으면 기본 색상으로 폴백
        Uint8 baseR, baseG, baseB;
        switch (wallType) {
            case 1: baseR = 139; baseG = 69; baseB = 19; break;  // 벽돌
            case 2: baseR = 105; baseG = 105; baseB = 105; break; // 돌
            case 3: baseR = 70; baseG = 70; baseB = 80; break;    // 금속
            default: baseR = 200; baseG = 200; baseB = 200; break;
        }
        
        Uint8 r = static_cast<Uint8>(baseR * lighting);
        Uint8 g = static_cast<Uint8>(baseG * lighting);
        Uint8 b = static_cast<Uint8>(baseB * lighting);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, x, wallTop, x, wallBottom);
    }
}

float Renderer::degreesToRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

void Renderer::renderFloorAndCeiling(Player* player, Map* map) {
    // 손전등 상태에 따른 극적인 조명 차이
    float avgLighting = 0.005f; // 기본적으로 거의 비어보이지 않음
    
    if (lightSystem->isFlashlightEnabled()) {
        // 손전등이 켜져있어도 바닥/천장은 약간만 보임
        avgLighting = 0.08f;
    }
    
    // 바닥 렌더링 (매우 어두운 사각형)
    Uint8 floorR = static_cast<Uint8>(60 * avgLighting);
    Uint8 floorG = static_cast<Uint8>(50 * avgLighting);
    Uint8 floorB = static_cast<Uint8>(40 * avgLighting);
    
    SDL_SetRenderDrawColor(renderer, floorR, floorG, floorB, 255);
    SDL_Rect floorRect = {0, screenHeight / 2, screenWidth, screenHeight / 2};
    SDL_RenderFillRect(renderer, &floorRect);
    
    // 천장 렌더링 (완전히 어두운 사각형)
    Uint8 ceilR = static_cast<Uint8>(30 * avgLighting);
    Uint8 ceilG = static_cast<Uint8>(25 * avgLighting);
    Uint8 ceilB = static_cast<Uint8>(20 * avgLighting);
    
    SDL_SetRenderDrawColor(renderer, ceilR, ceilG, ceilB, 255);
    SDL_Rect ceilingRect = {0, 0, screenWidth, screenHeight / 2};
    SDL_RenderFillRect(renderer, &ceilingRect);
}
