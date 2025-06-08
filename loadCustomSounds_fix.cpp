void Game::loadCustomSounds() {
    if (!audioManager || !audioManager->isInitialized()) return;
    
    std::cout << "🎵 Loading custom sounds..." << std::endl;
    
    // 절대 경로로 sounds 폴더 지정 (프로젝트 루트 기준)
    std::string soundsPath = "../sounds";  // build 디렉토리에서 상위 폴더의 sounds
    
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
    
    // 개별 파일 로딩 시도 (절대 경로 사용)
    if (audioManager->loadSoundFile("../sounds/footstep1.wav", SoundType::CUSTOM_FOOTSTEP_1)) {
        audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_1);
        std::cout << "✅ Custom footstep 1 loaded" << std::endl;
    }
    
    if (audioManager->loadSoundFile("../sounds/footstep2.wav", SoundType::CUSTOM_FOOTSTEP_2)) {
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
