#pragma once
#include <SDL2/SDL.h>
#include "Player.h"
#include "Map.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "HUD.h"
#include "LightSystem.h"
#include "AudioManager.h"
#include "ItemManager.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Player* player;
    Map* map;
    class Renderer* gameRenderer;
    TextureManager* textureManager;
    HUD* hud;
    LightSystem* lightSystem;
    AudioManager* audioManager;
    ItemManager* itemManager;
    
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    
    // FPS 계산
    Uint32 frameCount;
    Uint32 fpsTimer;
    float currentFPS;
    
    // 키 입력 상태 추적
    bool fKeyPressed;
    bool fKeyWasPressed;
    
    // 이동 상태 추적 (발자국 소리용)
    bool isMoving;
    bool wasMoving;
    
    // 타이밍 관련
    Uint32 lastFrameTime;
    
    // 게임 상태
    bool showLevelCompleteMessage;
    Uint32 levelCompleteTime;
    
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void handleEvents(float deltaTime);
    void update(float deltaTime);
    void render();
    void cleanup();
    void calculateFPS();
    
private:
    void loadCustomSounds(); // 커스텀 사운드 로딩
    void checkLevelCompletion();
    void handleLevelTransition();
};
