#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               player(nullptr), map(nullptr), gameRenderer(nullptr),
               textureManager(nullptr), hud(nullptr), lightSystem(nullptr),
               frameCount(0), fpsTimer(0), currentFPS(60.0f),
               fKeyPressed(false), fKeyWasPressed(false), lastFrameTime(0) {
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 윈도우 생성
    window = SDL_CreateWindow("Joom - Doom Clone",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 렌더러 생성 (VSync 활성화)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        // VSync 실패시 일반 가속 렌더러로 대체
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
    }
    
    // 게임 객체들 초기화
    map = new Map();
    player = new Player(3.0f, 3.0f, 0.0f);  // 맵 중앙에서 시작
    textureManager = new TextureManager(renderer);
    lightSystem = new LightSystem();
    gameRenderer = new Renderer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, textureManager, lightSystem);
    hud = new HUD(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 텍스처 초기화
    gameRenderer->initializeTextures();
    
    // FPS 타이머 초기화
    fpsTimer = SDL_GetTicks();
    lastFrameTime = SDL_GetTicks();
    
    running = true;
    return true;
}

void Game::run() {
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS; // 16.67ms per frame
    
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        
        // deltaTime 계산
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;
        
        // deltaTime 제한 (최소 1/120초, 최대 1/30초)
        deltaTime = std::max(1.0f / 120.0f, std::min(deltaTime, 1.0f / 30.0f));
        
        handleEvents(deltaTime);
        update(deltaTime);
        render();
        calculateFPS();
        
        // 프레임 제한
        int frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void Game::handleEvents(float deltaTime) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
    }
    
    // 키보드 상태 확인
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // deltaTime 사용으로 프레임률에 독립적인 움직임
    if (currentKeyStates[SDL_SCANCODE_W]) {
        player->moveForward(deltaTime, map);
    }
    if (currentKeyStates[SDL_SCANCODE_S]) {
        player->moveBackward(deltaTime, map);
    }
    if (currentKeyStates[SDL_SCANCODE_A]) {
        player->strafeLeft(deltaTime, map);
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
        player->strafeRight(deltaTime, map);
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT]) {
        player->rotateLeft(deltaTime);
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
        player->rotateRight(deltaTime);
    }
    if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }
    
    // F키 단일 누름 감지 (토글 방식)
    fKeyPressed = currentKeyStates[SDL_SCANCODE_F];
    if (fKeyPressed && !fKeyWasPressed) {
        lightSystem->toggleFlashlight();
    }
    fKeyWasPressed = fKeyPressed;
}

void Game::update(float deltaTime) {
    // 게임 로직 업데이트
    // HUD 업데이트
    hud->setFPS(currentFPS);
    hud->setFlashlightStatus(lightSystem->isFlashlightEnabled());
}

void Game::render() {
    gameRenderer->clear();
    gameRenderer->render3D(player, map);
    gameRenderer->renderMiniMap(player, map);
    hud->render();
    gameRenderer->present();
}

void Game::cleanup() {
    delete hud;
    delete gameRenderer;
    delete lightSystem;
    delete textureManager;
    delete player;
    delete map;
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}

void Game::calculateFPS() {
    frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    
    if (currentTime - fpsTimer >= 1000) { // 1초마다 업데이트
        currentFPS = frameCount * 1000.0f / (currentTime - fpsTimer);
        frameCount = 0;
        fpsTimer = currentTime;
    }
}
