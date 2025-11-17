#pragma once
#include <SDL2/SDL.h>
#include "Player.h"
#include "Map.h"
#include "TextureManager.h"
#include "LightSystem.h"
#include "ItemManager.h"

class Monster;

class Renderer {
public:
    Renderer(SDL_Renderer* sdlRenderer, int width, int height, TextureManager* texMgr, LightSystem* lights);
    ~Renderer();

    void initializeTextures();
    void render(Player* player, Map* map, const std::vector<Item>& items, const Monster* monster);
    void present();
    void renderMiniMap(Player* player, Map* map); // 미니맵은 버퍼링 없이 직접 렌더링

private:
    void renderFloorAndCeiling(Player* player, Uint32* pixels);
    void renderWalls(Player* player, Map* map, Uint32* pixels);
    void renderSprites(Player* player, const std::vector<Item>& items, const Monster* monster, Uint32* pixels);

    Uint32 applyLighting(Uint32 color, float lighting) const;

    SDL_Renderer* renderer;
    SDL_Texture* screenBuffer;
    int screenWidth, screenHeight;
    TextureManager* textureManager;
    LightSystem* lightSystem;
    std::vector<float> depthBuffer;

    // Profiling
    Uint32 profilingTimer;
    Uint32 wallTimeAccumulator;
    Uint32 floorTimeAccumulator;
    int frameCounterForProfile;

    static constexpr float FOV = 60.0f;
    float degreesToRadians(float degrees);
};
