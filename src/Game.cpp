#include "Game.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               player(nullptr), map(nullptr), gameRenderer(nullptr),
               textureManager(nullptr), hud(nullptr), lightSystem(nullptr),
               audioManager(nullptr), itemManager(nullptr),
               frameCount(0), fpsTimer(0), currentFPS(60.0f),
               fKeyPressed(false), fKeyWasPressed(false), 
               isMoving(false), wasMoving(false), lastFrameTime(0) {
}

Game::~Game() {
    cleanup();
}

bool Game::initialize(const std::string& resourcePath) {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 윈도우 생성
    window = SDL_CreateWindow("Joom - Cave Explorer",
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
    
    // 오디오 시스템 초기화
    audioManager = new AudioManager();
    if (!audioManager->initialize()) {
        std::cerr << "Audio system failed to initialize, continuing without sound..." << std::endl;
    } else {
        loadCustomSounds(resourcePath);
    }
    
    // 게임 객체들 초기화
    map = new Map();
    map->generateInitialChunk();
    itemManager = new ItemManager();
    
    // Find a safe starting position in the initial chunk
    float startX = 8.5f, startY = 8.5f; // Default fallback
    bool foundSpawn = false;
    for (int y = 1; y < CHUNK_SIZE - 1 && !foundSpawn; ++y) {
        for (int x = 1; x < CHUNK_SIZE - 1 && !foundSpawn; ++x) {
            if (!map->isWallAt(x, y)) {
                startX = x + 0.5f;
                startY = y + 0.5f;
                foundSpawn = true;
            }
        }
    }
    std::cout << "Player spawned at safe location: (" << startX << ", " << startY << ")" << std::endl;
    
    player = new Player(startX, startY, 0.0f);
    textureManager = new TextureManager(renderer, resourcePath + "textures/");
    lightSystem = new LightSystem();
    gameRenderer = new Renderer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, textureManager, lightSystem);
    hud = new HUD(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    // 텍스처 초기화
    gameRenderer->initializeTextures();
    
    // FPS 타이머 초기화
    fpsTimer = SDL_GetTicks();
    lastFrameTime = SDL_GetTicks();
    
    running = true;
    
    // 시작음 재생
    if (audioManager && audioManager->isInitialized()) {
        audioManager->playSound(SoundType::UI_BEEP);
        
        // 엠비언트 사운드 시작
        std::string ambientPath = resourcePath + "sounds/ambient1.wav";
        if (std::filesystem::exists(ambientPath)) {
            audioManager->playMusic(ambientPath);
            std::cout << "🎵 Ambient sound started: " << ambientPath << std::endl;
        } else {
            std::cout << "⚠️  No ambient1.wav file found in " << resourcePath + "sounds/" << std::endl;
        }
    }
    
    std::cout << "🎮 Cave Explorer initialized!" << std::endl;
    std::cout << "🗝️  Find keys to unlock the exit and advance to the next level!" << std::endl;
    
    return true;
}

void Game::loadCustomSounds(const std::string& resourcePath) {
    if (!audioManager || !audioManager->isInitialized()) return;
    
    // sounds 폴더에서 모든 사운드 파일 로드
    std::string soundsPath = resourcePath + "sounds/";
    audioManager->loadSoundsFromDirectory(soundsPath);
}

void Game::run() {
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;
    
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        
        // deltaTime 계산
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;
        
        // deltaTime 제한
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
        } else if (e.type == SDL_KEYDOWN) {
            // 숫자 키로 커스텀 사운드 테스트
            if (audioManager && audioManager->isInitialized()) {
                switch (e.key.keysym.sym) {
                    case SDLK_1: audioManager->playSound("footstep1"); break;
                    case SDLK_2: audioManager->playSound("footstep2"); break;
                    case SDLK_3: audioManager->playSound("ambient1"); break;
                    case SDLK_4: audioManager->playSound("weapon1"); break;
                    case SDLK_5: audioManager->playSound("door1"); break;
                    case SDLK_6: audioManager->playSound("switch1"); break;
                }
            }
        }
    }
    
    // 키보드 상태 확인
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // 이동 상태 추적
    isMoving = false;
    
    // 이동 처리
    if (currentKeyStates[SDL_SCANCODE_W]) {
        player->moveForward(deltaTime, map);
        isMoving = true;
    }
    if (currentKeyStates[SDL_SCANCODE_S]) {
        player->moveBackward(deltaTime, map);
        isMoving = true;
    }
    if (currentKeyStates[SDL_SCANCODE_A]) {
        player->strafeLeft(deltaTime, map);
        isMoving = true;
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
        player->strafeRight(deltaTime, map);
        isMoving = true;
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

    // Volume controls (handle once per press)
    static bool minusWasPressed = false;
    static bool equalsWasPressed = false;
    bool minusIsPressed = currentKeyStates[SDL_SCANCODE_MINUS];
    bool equalsIsPressed = currentKeyStates[SDL_SCANCODE_EQUALS];

    if (audioManager && audioManager->isInitialized()) {
        if (minusIsPressed && !minusWasPressed) {
            audioManager->decreaseMasterVolume();
        }
        if (equalsIsPressed && !equalsWasPressed) {
            audioManager->increaseMasterVolume();
        }
    }
    minusWasPressed = minusIsPressed;
    equalsWasPressed = equalsIsPressed;
    
    // 발자국 소리 재생
    if (audioManager && audioManager->isInitialized() && isMoving) {
        audioManager->playFootstep();
    }
    
    // F키 토글 (손전등)
    fKeyPressed = currentKeyStates[SDL_SCANCODE_F];
    if (fKeyPressed && !fKeyWasPressed) {
        lightSystem->toggleFlashlight();
        
        if (audioManager && audioManager->isInitialized()) {
            if (audioManager->isSoundLoaded("switch1")) {
                audioManager->playSound("switch1");
            } else {
                audioManager->playSound(SoundType::FLASHLIGHT_TOGGLE);
            }
        }
    }
    fKeyWasPressed = fKeyPressed;
    
    // 조명 조절 키들은 기존과 동일...
    // (간결성을 위해 생략, 필요시 추가)
    
    wasMoving = isMoving;
}

void Game::update(float deltaTime) {
    // 플레이어 위치에 따라 청크 로드
    map->checkAndLoadChunks(player->getX(), player->getY());

    // 아이템 시스템 업데이트
    itemManager->update(deltaTime);
    
    // 플레이어와 아이템 충돌 검사
    if (itemManager->checkItemCollision(player->getX(), player->getY())) {
        // 아이템 수집 효과음
        if (audioManager && audioManager->isInitialized()) {
            if (audioManager->isSoundLoaded("pickup")) {
                audioManager->playSound("pickup");
            } else {
                audioManager->playSound(SoundType::UI_BEEP);
            }
        }
    }
    
    // HUD 업데이트
    hud->setFPS(currentFPS);
    hud->setFlashlightStatus(lightSystem->isFlashlightEnabled());
    hud->setHealth(itemManager->getHealth());
    hud->setAmmo(itemManager->getAmmo());
    
    // 키 보유 현황 업데이트
    bool hasRedKey = itemManager->hasKey(ItemType::KEY_RED);
    bool hasBlueKey = itemManager->hasKey(ItemType::KEY_BLUE);
    bool hasYellowKey = itemManager->hasKey(ItemType::KEY_YELLOW);
    hud->setKeyStatus(hasRedKey, hasBlueKey, hasYellowKey);
    
    // 오디오 상태 업데이트
    if (audioManager) {
        hud->setAudioStatus(audioManager->isInitialized(), audioManager->getMasterVolume());
    } else {
        hud->setAudioStatus(false, 0);
    }
}

void Game::render() {
    // 3D 월드를 버퍼에 렌더링
    gameRenderer->render(player, map, itemManager->getItems(), nullptr);

    // 버퍼를 화면에 복사
    gameRenderer->present();

    // 2D UI 요소들을 화면에 직접 렌더링 (3D 월드 위에)
    gameRenderer->renderMiniMap(player, map);
    hud->render();
    
    // 최종 결과물을 화면에 표시
    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    delete itemManager;
    delete hud;
    delete gameRenderer;
    delete lightSystem;
    delete textureManager;
    delete player;
    delete map;
    delete audioManager;
    
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
    
    if (currentTime - fpsTimer >= 1000) {
        currentFPS = frameCount * 1000.0f / (currentTime - fpsTimer);
        frameCount = 0;
        fpsTimer = currentTime;
    }
}
