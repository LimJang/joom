#pragma once
#include <SDL2/SDL.h>
#include "Player.h"
#include "Map.h"
#include "TextureManager.h"
#include "LightSystem.h"

class Renderer {
private:
    SDL_Renderer* renderer;
    int screenWidth, screenHeight;
    TextureManager* textureManager;
    LightSystem* lightSystem;
    
    // 레이캐스팅 설정
    static constexpr float FOV = 60.0f;  // 시야각 (도)
    static constexpr float RAY_STEP = 0.1f;
    static constexpr float MAX_DISTANCE = 20.0f;
    
public:
    Renderer(SDL_Renderer* sdlRenderer, int width, int height, TextureManager* texMgr, LightSystem* lights);
    ~Renderer();
    
    // 텍스처 초기화
    void initializeTextures();
    
    void render3D(Player* player, Map* map);
    void renderMiniMap(Player* player, Map* map);
    void renderFloorAndCeiling(Player* player, Map* map);
    void clear();
    void present();
    
private:
    float castRay(float startX, float startY, float angle, Map* map, int& wallType, float& hitX, float& hitY);
    void drawTexturedVerticalLine(int x, int wallHeight, int wallType, float textureX, float lighting);
    void drawVerticalLine(int x, int height, int color);
    float degreesToRadians(float degrees);
};
