#include "Renderer.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(SDL_Renderer* sdlRenderer, int width, int height, TextureManager* texMgr, LightSystem* lights)
    : renderer(sdlRenderer), screenWidth(width), screenHeight(height), textureManager(texMgr), lightSystem(lights) {
    depthBuffer.resize(screenWidth);
}

Renderer::~Renderer() {
}

void Renderer::initializeTextures() {
    // 각 텍스처를 개별적으로 로딩 시도
    bool brick_loaded = textureManager->loadTexture("wall_brick", "wall_brick.png");
    bool stone_loaded = textureManager->loadTexture("wall_stone", "wall_stone.png");
    bool metal_loaded = textureManager->loadTexture("wall_metal", "wall_metal.png");
    
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
    
    // 뎁스 버퍼 초기화
    std::fill(depthBuffer.begin(), depthBuffer.end(), MAX_DISTANCE);

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
        
        // 뎁스 버퍼에 거리 기록
        depthBuffer[x] = distance;

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

void Renderer::renderItem(const Item& item, Player* player, LightSystem* lightSystem) {
    if (item.collected) return;
    
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    
    float deltaX = item.x - playerX;
    float deltaY = item.y - playerY;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    
    if (distance > 15.0f) return;
    
    float itemAngle = atan2(deltaY, deltaX);
    float angleDiff = itemAngle - playerAngle;
    
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
    
    float halfFOV = degreesToRadians(FOV / 2);
    if (abs(angleDiff) > halfFOV) return;
    
    float lighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, item.x, item.y, distance);
    if (lighting < 0.1f) return;
    
    float screenRatio = angleDiff / halfFOV;
    int screenX = screenWidth / 2 + static_cast<int>(screenRatio * screenWidth / 2);
    
    int itemSize = static_cast<int>(1000 / distance);
    if (item.type == ItemType::KEY_RED || item.type == ItemType::KEY_BLUE || item.type == ItemType::KEY_YELLOW) {
        itemSize = static_cast<int>(1200 / distance);
    }
    itemSize = std::max(6, std::min(itemSize, 40));
    
    int screenY = screenHeight / 2;
    
    // 뎁스 체크하며 아이템 그리기
    int itemWidth = itemSize;
    for (int x = screenX - itemWidth / 2; x < screenX + itemWidth / 2; ++x) {
        if (x >= 0 && x < screenWidth && distance < depthBuffer[x]) {
            // drawItemSprite는 이제 한 줄만 그리도록 수정되어야 함
            drawItemSprite(x, screenY, itemSize, item.type, lighting, item.animationTime);
        }
    }
}

void Renderer::drawItemSprite(int screenX, int screenY, int size, ItemType type, float lighting, float animationTime) {
    // 이 함수는 이제 한 줄만 그림
    float bobOffset = sin(animationTime * 3.0f) * 3.0f;
    screenY += static_cast<int>(bobOffset);
    
    SDL_Color color = getItemColor(type);
    
    Uint8 r = static_cast<Uint8>(color.r * lighting);
    Uint8 g = static_cast<Uint8>(color.g * lighting);
    Uint8 b = static_cast<Uint8>(color.b * lighting);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    int startY = screenY - size / 2;
    int endY = screenY + size / 2;
    
    // 아이템 타입에 따라 다른 모양을 그릴 수 있지만, 여기서는 간단히 수직선만 그림
    SDL_RenderDrawLine(renderer, screenX, startY, screenX, endY);
}

SDL_Color Renderer::getItemColor(ItemType type) {
    switch (type) {
        case ItemType::KEY_RED:
            return {255, 80, 80, 255};   // 더 밝은 빨간색
        case ItemType::KEY_BLUE:
            return {80, 150, 255, 255};  // 더 밝은 파란색
        case ItemType::KEY_YELLOW:
            return {255, 255, 80, 255};  // 더 밝은 노란색
        case ItemType::HEALTH_PACK:
            return {100, 255, 100, 255}; // 초록색
        case ItemType::AMMO_PACK:
            return {200, 200, 100, 255}; // 황금색
        case ItemType::EXIT_PORTAL:
            return {200, 100, 255, 255}; // 보라색
        default:
            return {255, 255, 255, 255}; // 흰색
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
            
            if (tileType == 9) {  // 출구
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);  // 마젠타
            } else if (tileType != 0) {  // 벽이면
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

void Renderer::drawKeySprite(int screenX, int screenY, int size, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    // 키 본체 (직사각형 모양)
    int keyWidth = size * 3 / 4;
    int keyHeight = size / 3;
    
    // 메인 키 모드 (key head)
    SDL_Rect keyHead = {
        screenX - keyWidth/2,
        screenY - keyHeight/2,
        keyWidth,
        keyHeight
    };
    
    // 키 모드 외곽선
    SDL_RenderDrawRect(renderer, &keyHead);
    
    // 키 모드 내부 채우기 (반투명)
    Uint8 fillR = r / 2;
    Uint8 fillG = g / 2; 
    Uint8 fillB = b / 2;
    SDL_SetRenderDrawColor(renderer, fillR, fillG, fillB, 128);
    
    for (int i = 1; i < keyWidth - 1; i++) {
        for (int j = 1; j < keyHeight - 1; j++) {
            SDL_RenderDrawPoint(renderer, keyHead.x + i, keyHead.y + j);
        }
    }
    
    // 키 샤프트 (key shaft)
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    int shaftLength = size / 2;
    int shaftWidth = size / 8;
    
    SDL_Rect keyShaft = {
        screenX - shaftWidth/2,
        screenY + keyHeight/2,
        shaftWidth,
        shaftLength
    };
    
    SDL_RenderFillRect(renderer, &keyShaft);
    
    // 키 이빨 (key teeth) - 오른쪽에 작은 이빨들
    int teethSize = size / 12;
    
    // 첫 번째 이빨
    SDL_Rect tooth1 = {
        screenX + shaftWidth/2,
        screenY + keyHeight/2 + shaftLength/3,
        teethSize,
        teethSize
    };
    SDL_RenderFillRect(renderer, &tooth1);
    
    // 두 번째 이빨
    SDL_Rect tooth2 = {
        screenX + shaftWidth/2,
        screenY + keyHeight/2 + 2*shaftLength/3,
        teethSize * 2,
        teethSize
    };
    SDL_RenderFillRect(renderer, &tooth2);
    
    // 키 구멍 (키 모드 중앙에 작은 원)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // 어두운 색
    int holeRadius = size / 12;
    
    for (int dx = -holeRadius; dx <= holeRadius; dx++) {
        for (int dy = -holeRadius; dy <= holeRadius; dy++) {
            if (dx*dx + dy*dy <= holeRadius*holeRadius) {
                SDL_RenderDrawPoint(renderer, screenX + dx, screenY + dy);
            }
        }
    }
    
    // 키 반짝임 효과 (상단 에지에 밝은 선)
    SDL_SetRenderDrawColor(renderer, 
        std::min(255, static_cast<int>(r * 1.5f)), 
        std::min(255, static_cast<int>(g * 1.5f)), 
        std::min(255, static_cast<int>(b * 1.5f)), 255);
    
    // 상단 가장자리 밝은 선
    SDL_RenderDrawLine(renderer, 
        keyHead.x + 1, keyHead.y + 1,
        keyHead.x + keyWidth - 2, keyHead.y + 1);
    
    // 왼쪽 가장자리 밝은 선
    SDL_RenderDrawLine(renderer, 
        keyHead.x + 1, keyHead.y + 1,
        keyHead.x + 1, keyHead.y + keyHeight - 2);
}

#include "Monster.h"

void Renderer::renderMonster(const Monster* monster, Player* player, LightSystem* lightSystem) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();

    float monsterX = monster->getX();
    float monsterY = monster->getY();

    float deltaX = monsterX - playerX;
    float deltaY = monsterY - playerY;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (distance > 15.0f) return;

    float monsterAngle = atan2(deltaY, deltaX);
    float angleDiff = monsterAngle - playerAngle;

    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;

    float halfFOV = degreesToRadians(FOV / 2);
    if (abs(angleDiff) > halfFOV) return;

    float lighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, monsterX, monsterY, distance);
    if (lighting < 0.1f) return;

    float screenRatio = angleDiff / halfFOV;
    int screenX = screenWidth / 2 + static_cast<int>(screenRatio * screenWidth / 2);

    int monsterSize = static_cast<int>(screenHeight / distance * 0.8f);
    monsterSize = std::max(10, std::min(monsterSize, 350)); // 최대 크기 증가

    int screenY = screenHeight / 2 + static_cast<int>(monsterSize * 0.25f);

    // 뎁스 체크하며 몬스터 그리기
    int monsterWidth = monsterSize;
    for (int x = screenX - monsterWidth / 2; x < screenX + monsterWidth / 2; ++x) {
        if (x >= 0 && x < screenWidth && distance < depthBuffer[x]) {
            // drawMonsterSprite는 이제 전체 스프라이트를 그림
        }
    }
    // 뎁스 체크는 전체 스프라이트에 대해 한 번만 수행
    if (distance < depthBuffer[screenX]) {
        drawMonsterSprite(screenX, screenY, monsterSize, lighting, monster->getAnimationTime());
    }
}

void Renderer::drawMonsterSprite(int screenX, int screenY, int size, float lighting, float animationTime) {
    // 애니메이션
    float bobbing = sin(animationTime * 2.0f) * (size * 0.05f);
    screenY += static_cast<int>(bobbing);

    // 몸체
    SDL_Color bodyColor = {50, 50, 60, 255}; // 어두운 금속색
    Uint8 bodyR = static_cast<Uint8>(bodyColor.r * lighting);
    Uint8 bodyG = static_cast<Uint8>(bodyColor.g * lighting);
    Uint8 bodyB = static_cast<Uint8>(bodyColor.b * lighting);
    SDL_SetRenderDrawColor(renderer, bodyR, bodyG, bodyB, 255);
    SDL_Rect bodyRect = {
        screenX - size / 4,
        screenY - size / 2,
        size / 2,
        size
    };
    SDL_RenderFillRect(renderer, &bodyRect);

    // 날개 애니메이션
    float wingFlap = sin(animationTime * 4.0f) * (size * 0.1f);

    // 왼쪽 날개
    SDL_Color wingColor = {80, 80, 90, 255}; // 몸체보다 밝은 금속색
    Uint8 wingR = static_cast<Uint8>(wingColor.r * lighting);
    Uint8 wingG = static_cast<Uint8>(wingColor.g * lighting);
    Uint8 wingB = static_cast<Uint8>(wingColor.b * lighting);
    SDL_SetRenderDrawColor(renderer, wingR, wingG, wingB, 255);
    SDL_Rect leftWingRect = {
        screenX - size / 2,
        screenY - size / 3 + static_cast<int>(wingFlap),
        size / 4,
        size / 2
    };
    SDL_RenderFillRect(renderer, &leftWingRect);

    // 오른쪽 날개
    SDL_Rect rightWingRect = {
        screenX + size / 4,
        screenY - size / 3 - static_cast<int>(wingFlap),
        size / 4,
        size / 2
    };
    SDL_RenderFillRect(renderer, &rightWingRect);

    // 중앙 코어(눈)
    float coreGlow = (sin(animationTime * 3.0f) + 1.0f) / 2.0f; // 0.0 ~ 1.0
    SDL_Color coreColor = {255, 50, 50, 255}; // 밝은 붉은색
    Uint8 coreR = static_cast<Uint8>(coreColor.r * lighting);
    Uint8 coreG = static_cast<Uint8>(coreColor.g * lighting * coreGlow);
    Uint8 coreB = static_cast<Uint8>(coreColor.b * lighting * coreGlow);
    SDL_SetRenderDrawColor(renderer, coreR, coreG, coreB, 255);
    SDL_Rect coreRect = {
        screenX - size / 8,
        screenY - size / 8,
        size / 4,
        size / 4
    };
    SDL_RenderFillRect(renderer, &coreRect);
}

void Renderer::renderFloorAndCeiling(Player* player, Map* map) {
    float playerAngle = player->getAngle();
    float fovRadians = degreesToRadians(FOV);

    // 화면의 각 수평 라인에 대해 반복 (중앙부터 바닥까지)
    for (int y = screenHeight / 2; y < screenHeight; ++y) {
        // 현재 y 라인에 대한 실제 세계에서의 거리 계산
        // (y - screenHeight / 2)가 0이 되는 것을 방지
        float screenY = y - screenHeight / 2.0f;
        if (screenY == 0) screenY = 1.0f;

        // 플레이어 높이를 0.5로 가정
        float distance = (0.5f * screenHeight) / screenY;

        // 거리에 따른 조명 값 가져오기 (LUT 사용)
        float lighting = 0.0f;
        if (lightSystem->isFlashlightEnabled()) {
            lighting = lightSystem->getLightFromDistanceLUT(distance);
        }
        
        // 환경광 추가
        lighting += lightSystem->getAmbientLight();
        lighting = std::clamp(lighting, 0.0f, 1.0f);

        // 바닥 색상 결정 (어두운 흙색 계열)
        Uint8 floorR = static_cast<Uint8>(std::clamp(60 * lighting, 0.0f, 255.0f));
        Uint8 floorG = static_cast<Uint8>(std::clamp(50 * lighting, 0.0f, 255.0f));
        Uint8 floorB = static_cast<Uint8>(std::clamp(40 * lighting, 0.0f, 255.0f));
        
        // 천장 색상 결정 (매우 어두운 회색 계열)
        Uint8 ceilR = static_cast<Uint8>(std::clamp(30 * lighting, 0.0f, 255.0f));
        Uint8 ceilG = static_cast<Uint8>(std::clamp(25 * lighting, 0.0f, 255.0f));
        Uint8 ceilB = static_cast<Uint8>(std::clamp(20 * lighting, 0.0f, 255.0f));

        // 바닥 라인 그리기
        SDL_SetRenderDrawColor(renderer, floorR, floorG, floorB, 255);
        SDL_RenderDrawLine(renderer, 0, y, screenWidth, y);

        // 천장 라인 그리기 (대칭 위치)
        SDL_SetRenderDrawColor(renderer, ceilR, ceilG, ceilB, 255);
        SDL_RenderDrawLine(renderer, 0, screenHeight - y, screenWidth, screenHeight - y);
    }
}
