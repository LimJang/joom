#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               player(nullptr), map(nullptr), gameRenderer(nullptr),
               textureManager(nullptr), hud(nullptr), lightSystem(nullptr),
               audioManager(nullptr), frameCount(0), fpsTimer(0), currentFPS(60.0f),
               fKeyPressed(false), fKeyWasPressed(false), 
               isMoving(false), wasMoving(false), lastFrameTime(0) {
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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
    
    // 오디오 시스템 초기화
    audioManager = new AudioManager();
    if (!audioManager->initialize()) {
        std::cerr << "⚠️  Audio system failed to initialize, continuing without sound..." << std::endl;
        // 오디오 없이도 게임을 계속 진행
    } else {
        // 커스텀 사운드 로딩 시도
        loadCustomSounds();
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
    
    // 시작음 재생
    if (audioManager && audioManager->isInitialized()) {
        audioManager->playSound(SoundType::UI_BEEP);
        std::cout << "🎮 Game initialized with audio support!" << std::endl;
    } else {
        std::cout << "🎮 Game initialized (silent mode)" << std::endl;
    }
    
    return true;
}

void Game::loadCustomSounds() {
    if (!audioManager || !audioManager->isInitialized()) return;
    
    std::cout << "🎵 Loading custom sounds..." << std::endl;
    
    // 현재 디렉토리(build)의 sounds 폴더 사용
    std::string soundsPath = "sounds";
    
    // sounds 폴더에서 모든 사운드 파일 로드
    if (audioManager->loadSoundsFromDirectory(soundsPath)) {
        std::cout << "✅ Custom sounds loaded from 'sounds' directory" << std::endl;
        
        // 로딩된 사운드 목록 출력
        auto soundNames = audioManager->getLoadedSoundNames();
        if (!soundNames.empty()) {
            std::cout << "📋 Available custom sounds:" << std::endl;
            for (const auto& name : soundNames) {
                std::cout << "  - " << name << std::endl;
            }
        }
    } else {
        std::cout << "ℹ️  No custom sounds found in 'sounds' directory" << std::endl;
        std::cout << "💡 You can add your own .wav, .mp3, .ogg files to 'sounds/' folder" << std::endl;
    }
    
    // 개별 파일 로딩 시도
    if (audioManager->loadSoundFile("sounds/footstep1.wav", SoundType::CUSTOM_FOOTSTEP_1)) {
        audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_1);
        std::cout << "✅ Custom footstep 1 loaded" << std::endl;
    }
    
    if (audioManager->loadSoundFile("sounds/footstep2.wav", SoundType::CUSTOM_FOOTSTEP_2)) {
        audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_2);
        std::cout << "✅ Custom footstep 2 loaded" << std::endl;
    }
    
    // 커스텀 발자국이 있으면 활성화
    if (audioManager->isSoundLoaded(SoundType::CUSTOM_FOOTSTEP_1) || 
        audioManager->isSoundLoaded(SoundType::CUSTOM_FOOTSTEP_2)) {
        audioManager->enableCustomFootsteps(true);
        std::cout << "🦶 Custom footsteps enabled!" << std::endl;
    }
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
        } else if (e.type == SDL_KEYDOWN) {
            // 숫자 키로 커스텀 사운드 테스트
            if (audioManager && audioManager->isInitialized()) {
                switch (e.key.keysym.sym) {
                    case SDLK_1:
                        audioManager->playSound("footstep1");
                        break;
                    case SDLK_2:
                        audioManager->playSound("footstep2");
                        break;
                    case SDLK_3:
                        audioManager->playSound("ambient1");
                        break;
                    case SDLK_4:
                        audioManager->playSound("weapon1");
                        break;
                    case SDLK_5:
                        audioManager->playSound("door1");
                        break;
                    case SDLK_6:
                        audioManager->playSound("switch1");
                        break;
                }
            }
        }
    }
    
    // 키보드 상태 확인
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // 이동 상태 추적
    isMoving = false;
    
    // deltaTime 사용으로 프레임률에 독립적인 움직임
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
    
    // 발자국 소리 재생
    if (audioManager && audioManager->isInitialized()) {
        if (isMoving) {
            audioManager->playFootstep();
        }
    }
    
    // F키 단일 누름 감지 (토글 방식)
    fKeyPressed = currentKeyStates[SDL_SCANCODE_F];
    if (fKeyPressed && !fKeyWasPressed) {
        lightSystem->toggleFlashlight();
        
        // 손전등 토글 사운드 재생 (switch1.wav 우선, 없으면 기본 사운드)
        if (audioManager && audioManager->isInitialized()) {
            if (audioManager->isSoundLoaded("switch1")) {
                audioManager->playSound("switch1");
            } else {
                audioManager->playSound(SoundType::FLASHLIGHT_TOGGLE);
            }
        }
    }
    fKeyWasPressed = fKeyPressed;
    
    // C키로 커스텀 발자국 토글
    static bool cKeyPressed = false;
    static bool cKeyWasPressed = false;
    cKeyPressed = currentKeyStates[SDL_SCANCODE_C];
    if (cKeyPressed && !cKeyWasPressed) {
        if (audioManager && audioManager->isInitialized()) {
            bool current = audioManager->isUsingCustomFootsteps();
            audioManager->enableCustomFootsteps(!current);
            std::cout << "🦶 Custom footsteps: " << (!current ? "ENABLED" : "DISABLED") << std::endl;
        }
    }
    cKeyWasPressed = cKeyPressed;
    
    // 조명 조절 키 ([/] 키로 환경광 조절)
    static bool leftBracketPressed = false;
    static bool rightBracketPressed = false;
    static bool leftBracketWasPressed = false;
    static bool rightBracketWasPressed = false;
    
    leftBracketPressed = currentKeyStates[SDL_SCANCODE_LEFTBRACKET];
    rightBracketPressed = currentKeyStates[SDL_SCANCODE_RIGHTBRACKET];
    
    if (leftBracketPressed && !leftBracketWasPressed) {
        float currentAmbient = lightSystem->getAmbientLight();
        lightSystem->setAmbientLight(std::max(0.0f, currentAmbient - 0.1f));
        std::cout << "🌙 환경광 감소: " << lightSystem->getAmbientLight() << std::endl;
    }
    if (rightBracketPressed && !rightBracketWasPressed) {
        float currentAmbient = lightSystem->getAmbientLight();
        lightSystem->setAmbientLight(std::min(1.0f, currentAmbient + 0.1f));
        std::cout << "☀️ 환경광 증가: " << lightSystem->getAmbientLight() << std::endl;
    }
    
    leftBracketWasPressed = leftBracketPressed;
    rightBracketWasPressed = rightBracketPressed;
    
    // 손전등 강도 조절 (-/= 키)
    static bool minusPressed = false;
    static bool equalsPressed = false;
    static bool minusWasPressed = false;
    static bool equalsWasPressed = false;
    
    minusPressed = currentKeyStates[SDL_SCANCODE_MINUS];
    equalsPressed = currentKeyStates[SDL_SCANCODE_EQUALS];
    
    if (minusPressed && !minusWasPressed) {
        float currentIntensity = lightSystem->getFlashlightIntensity();
        lightSystem->setFlashlightIntensity(std::max(0.0f, currentIntensity - 0.1f));
        std::cout << "🔦 손전등 강도 감소: " << lightSystem->getFlashlightIntensity() << std::endl;
    }
    if (equalsPressed && !equalsWasPressed) {
        float currentIntensity = lightSystem->getFlashlightIntensity();
        lightSystem->setFlashlightIntensity(std::min(2.0f, currentIntensity + 0.1f));
        std::cout << "🔦 손전등 강도 증가: " << lightSystem->getFlashlightIntensity() << std::endl;
    }
    
    minusWasPressed = minusPressed;
    equalsWasPressed = equalsPressed;
    
    wasMoving = isMoving;
}

void Game::update(float deltaTime) {
    // 게임 로직 업데이트
    // HUD 업데이트
    hud->setFPS(currentFPS);
    hud->setFlashlightStatus(lightSystem->isFlashlightEnabled());
    
    // 오디오 상태 업데이트
    if (audioManager) {
        hud->setAudioStatus(audioManager->isInitialized(), audioManager->getMasterVolume());
    } else {
        hud->setAudioStatus(false, 0);
    }
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
    
    if (currentTime - fpsTimer >= 1000) { // 1초마다 업데이트
        currentFPS = frameCount * 1000.0f / (currentTime - fpsTimer);
        frameCount = 0;
        fpsTimer = currentTime;
    }
}
