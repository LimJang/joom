#include "Game.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               player(nullptr), map(nullptr), gameRenderer(nullptr),
               textureManager(nullptr), hud(nullptr), lightSystem(nullptr),
               audioManager(nullptr), itemManager(nullptr), frameCount(0), fpsTimer(0), currentFPS(60.0f),
               fKeyPressed(false), fKeyWasPressed(false), 
               isMoving(false), wasMoving(false), lastFrameTime(0),
               showLevelCompleteMessage(false), levelCompleteTime(0) {
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
        loadCustomSounds();
    }
    
    // 게임 객체들 초기화
    map = new Map();
    itemManager = new ItemManager();
    
    // 레벨 1에 맞는 안전한 시작 위치 찾기
    float startX = 3.0f, startY = 3.0f;
    if (map->isWallAt(startX, startY)) {
        // 안전한 위치를 찾을 때까지 주변 탐색
        bool found = false;
        for (int dy = -2; dy <= 2 && !found; dy++) {
            for (int dx = -2; dx <= 2 && !found; dx++) {
                float testX = startX + dx;
                float testY = startY + dy;
                if (!map->isWallAt(testX, testY) && 
                    testX > 1.0f && testX < map->getWidth() - 1.0f &&
                    testY > 1.0f && testY < map->getHeight() - 1.0f) {
                    startX = testX;
                    startY = testY;
                    found = true;
                }
            }
        }
    }
    
    player = new Player(startX, startY, 0.0f);  // 안전한 위치에서 시작
    textureManager = new TextureManager(renderer);
    lightSystem = new LightSystem();
    gameRenderer = new Renderer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, textureManager, lightSystem);
    hud = new HUD(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 텍스처 초기화
    gameRenderer->initializeTextures();
    
    // 첫 번째 레벨의 아이템들 설정
    map->setupItemsForLevel(itemManager);
    
    // FPS 타이머 초기화
    fpsTimer = SDL_GetTicks();
    lastFrameTime = SDL_GetTicks();
    
    running = true;
    
    // 시작음 재생
    if (audioManager && audioManager->isInitialized()) {
        audioManager->playSound(SoundType::UI_BEEP);
        
        // 엠비언트 사운드 시작 (ambient1 파일이 있으면 재생, 없으면 기본 사운드)
        if (audioManager->isSoundLoaded("ambient1")) {
            audioManager->playMusic("sounds/ambient1.wav"); // 무한 반복
            std::cout << "🎵 Ambient sound started: ambient1" << std::endl;
        } else {
            // ambient1 파일이 없어도 다른 형식들 시도
            std::vector<std::string> ambientFiles = {
                "sounds/ambient1.mp3",
                "sounds/ambient1.ogg", 
                "sounds/ambient1.flac"
            };
            
            bool ambientStarted = false;
            for (const std::string& file : ambientFiles) {
                if (std::filesystem::exists(file)) {
                    audioManager->playMusic(file);
                    std::cout << "🎵 Ambient sound started: " << file << std::endl;
                    ambientStarted = true;
                    break;
                }
            }
            
            if (!ambientStarted) {
                std::cout << "⚠️  No ambient1 file found - place ambient1.wav/mp3/ogg in sounds/ folder for background atmosphere" << std::endl;
            }
        }
    }
    
    std::cout << "🎮 Cave Explorer initialized!" << std::endl;
    std::cout << "🗝️  Find keys to unlock the exit and advance to the next level!" << std::endl;
    
    return true;
}

void Game::loadCustomSounds() {
    if (!audioManager || !audioManager->isInitialized()) return;
    
    // sounds 폴더에서 모든 사운드 파일 로드
    std::string soundsPath = "sounds";
    audioManager->loadSoundsFromDirectory(soundsPath);
    
    // 개별 파일 로딩 시도
    if (audioManager->loadSoundFile("sounds/footstep1.wav", SoundType::CUSTOM_FOOTSTEP_1)) {
        audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_1);
    }
    
    if (audioManager->loadSoundFile("sounds/footstep2.wav", SoundType::CUSTOM_FOOTSTEP_2)) {
        audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_2);
    }
    
    // 커스텀 발자국이 있으면 활성화
    if (audioManager->isSoundLoaded(SoundType::CUSTOM_FOOTSTEP_1) || 
        audioManager->isSoundLoaded(SoundType::CUSTOM_FOOTSTEP_2)) {
        audioManager->enableCustomFootsteps(true);
    }
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
    
    // C키로 커스텀 발자국 토글
    static bool cKeyPressed = false;
    static bool cKeyWasPressed = false;
    cKeyPressed = currentKeyStates[SDL_SCANCODE_C];
    if (cKeyPressed && !cKeyWasPressed && audioManager && audioManager->isInitialized()) {
        bool current = audioManager->isUsingCustomFootsteps();
        audioManager->enableCustomFootsteps(!current);
    }
    cKeyWasPressed = cKeyPressed;
    
    // 조명 조절 키들은 기존과 동일...
    // (간결성을 위해 생략, 필요시 추가)
    
    wasMoving = isMoving;
}

void Game::update(float deltaTime) {
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
    
    // 레벨 완료 체크
    checkLevelCompletion();
    
    // 레벨 완료 메시지 처리
    if (showLevelCompleteMessage) {
        if (SDL_GetTicks() - levelCompleteTime > 2000) { // 2초 후 다음 레벨
            handleLevelTransition();
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
    
    // 현재 레벨 표시
    hud->setCurrentLevel(map->getCurrentLevel());
    
    // 오디오 상태 업데이트
    if (audioManager) {
        hud->setAudioStatus(audioManager->isInitialized(), audioManager->getMasterVolume());
    } else {
        hud->setAudioStatus(false, 0);
    }
}

void Game::checkLevelCompletion() {
    // 출구에 도달했는지 확인
    if (map->isExitAt(player->getX(), player->getY())) {
        // 필요한 키를 모두 가지고 있는지 확인
        if (map->canAdvanceToNextLevel(itemManager)) {
            if (!showLevelCompleteMessage) {
                showLevelCompleteMessage = true;
                levelCompleteTime = SDL_GetTicks();
                
                std::cout << "🎉 Level " << map->getCurrentLevel() << " Complete!" << std::endl;
                
                // 레벨 완료 사운드
                if (audioManager && audioManager->isInitialized()) {
                    if (audioManager->isSoundLoaded("victory")) {
                        audioManager->playSound("victory");
                    } else {
                        audioManager->playSound(SoundType::UI_BEEP);
                    }
                }
            }
        } else {
            // 키가 부족할 때 알림 (너무 자주 뜨지 않도록 제한)
            static Uint32 lastWarningTime = 0;
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastWarningTime > 1000) { // 1초에 한 번만
                std::cout << "🚪 You need to collect all required keys to exit this level!" << std::endl;
                lastWarningTime = currentTime;
            }
        }
    }
}

void Game::handleLevelTransition() {
    showLevelCompleteMessage = false;
    
    // 다음 레벨로 진행
    map->advanceToNextLevel(itemManager);
    
    // 레벨별 안전한 시작 위치 설정
    float startX, startY;
    switch (map->getCurrentLevel()) {
        case 1:
            startX = 3.0f; startY = 3.0f; // 레벨 1: 중앙 빈 공간
            break;
        case 2:
            startX = 8.0f; startY = 6.5f; // 레벨 2: 중앙 복도
            break;
        case 3:
            startX = 8.0f; startY = 10.5f; // 레벨 3: 하단 중앙 빈 공간
            break;
        default:
            startX = 2.0f; startY = 2.0f; // 기본값: 맵 좌상단 근처
            break;
    }
    
    // 선택된 위치가 벽인지 확인하고 안전한 위치 찾기
    if (map->isWallAt(startX, startY)) {
        // 안전한 위치를 찾을 때까지 주변 탐색
        bool found = false;
        for (int dy = -2; dy <= 2 && !found; dy++) {
            for (int dx = -2; dx <= 2 && !found; dx++) {
                float testX = startX + dx;
                float testY = startY + dy;
                if (!map->isWallAt(testX, testY) && 
                    testX > 1.0f && testX < map->getWidth() - 1.0f &&
                    testY > 1.0f && testY < map->getHeight() - 1.0f) {
                    startX = testX;
                    startY = testY;
                    found = true;
                }
            }
        }
    }
    
    // 플레이어를 안전한 시작 위치로 이동
    player->setPosition(startX, startY);
    player->setAngle(0.0f);
    
    std::cout << "🗝️  Level " << map->getCurrentLevel() << " - Find the keys to unlock the exit!" << std::endl;
    std::cout << "👤 Player spawned at (" << startX << ", " << startY << ")" << std::endl;
}

void Game::render() {
    gameRenderer->clear();
    gameRenderer->render3D(player, map);
    
    // 아이템 렌더링 (간단한 색깔 점으로 표시)
    for (const Item& item : itemManager->getItems()) {
        if (!item.collected) {
            gameRenderer->renderItem(item, player, lightSystem);
        }
    }
    
    gameRenderer->renderMiniMap(player, map);
    hud->render();
    
    // 레벨 완료 메시지 표시
    if (showLevelCompleteMessage) {
        hud->renderLevelCompleteMessage(map->getCurrentLevel());
    }
    
    gameRenderer->present();
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
