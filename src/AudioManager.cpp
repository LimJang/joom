#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <filesystem>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AudioManager::AudioManager() 
    : backgroundMusic(nullptr), initialized(false), 
      masterVolume(70), sfxVolume(80), musicVolume(50),
      lastFootstepTime(0), useCustomFootsteps(false) {
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize() {
    // SDL_mixer 초기화
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    // 믹싱 채널 할당
    Mix_AllocateChannels(16);
    
    initialized = true;
    
    // 절차적 사운드 생성
    generateSounds();
    
    // 볼륨 설정 적용
    setMasterVolume(masterVolume);
    setSFXVolume(sfxVolume);
    setMusicVolume(musicVolume);
    
    return true;
}

void AudioManager::cleanup() {
    if (!initialized) return;
    
    // 기본 사운드 해제
    for (auto& pair : sounds) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds.clear();
    
    // 네임드 사운드 해제
    for (auto& pair : namedSounds) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    namedSounds.clear();
    
    // 음악 해제
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    
    // SDL_mixer 종료
    Mix_CloseAudio();
    initialized = false;
}

void AudioManager::generateSounds() {
    if (!initialized) return;
    
    // 구두 발자국 소리 (또각또각)
    sounds[SoundType::FOOTSTEP_STONE] = generateShoeClickSound();
    
    // 손전등 토글 소리
    sounds[SoundType::FLASHLIGHT_TOGGLE] = generateFlashlightSound();
    
    // UI 비프음
    sounds[SoundType::UI_BEEP] = generateUIBeepSound();
    
    // 리버브 구두 소리 (에코 없이 리버브만)
    sounds[SoundType::ECHO_FOOTSTEP] = generateReverbShoeSound();
}

bool AudioManager::loadSoundFile(const std::string& filePath, SoundType soundType) {
    if (!initialized) {
        std::cerr << "Audio system not initialized!" << std::endl;
        return false;
    }
    
    if (!isValidAudioFile(filePath)) {
        std::cerr << "Invalid audio file: " << filePath << std::endl;
        return false;
    }
    
    // 기존 사운드가 있다면 해제
    auto it = sounds.find(soundType);
    if (it != sounds.end() && it->second) {
        Mix_FreeChunk(it->second);
    }
    
    // 새 사운드 로드
    Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound: " << filePath << " - " << Mix_GetError() << std::endl;
        return false;
    }
    
    sounds[soundType] = chunk;
    return true;
}

bool AudioManager::loadSoundFile(const std::string& filePath, const std::string& soundName) {
    if (!initialized) {
        std::cerr << "Audio system not initialized!" << std::endl;
        return false;
    }
    
    if (!isValidAudioFile(filePath)) {
        std::cerr << "Invalid audio file: " << filePath << std::endl;
        return false;
    }
    
    // 기존 사운드가 있다면 해제
    auto it = namedSounds.find(soundName);
    if (it != namedSounds.end() && it->second) {
        Mix_FreeChunk(it->second);
    }
    
    // 새 사운드 로드
    Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound: " << filePath << " - " << Mix_GetError() << std::endl;
        return false;
    }
    
    namedSounds[soundName] = chunk;
    return true;
}

bool AudioManager::loadSoundsFromDirectory(const std::string& directoryPath) {
    if (!initialized) {
        std::cerr << "Audio system not initialized!" << std::endl;
        return false;
    }
    
    namespace fs = std::filesystem;
    
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        std::cerr << "Directory not found: " << directoryPath << std::endl;
        return false;
    }
    
    int loadedCount = 0;
    
    try {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                
                if (isValidAudioFile(filePath)) {
                    std::string soundName = getFileNameWithoutExtension(filePath);
                    
                    if (loadSoundFile(filePath, soundName)) {
                        loadedCount++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
        return false;
    }
    
    return loadedCount > 0;
}

void AudioManager::playSound(SoundType soundType) {
    if (!initialized) return;
    
    auto it = sounds.find(soundType);
    if (it != sounds.end() && it->second) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioManager::playSound(const std::string& soundName) {
    if (!initialized) return;
    
    auto it = namedSounds.find(soundName);
    if (it != namedSounds.end() && it->second) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioManager::playFootstep() {
    if (!initialized) return;
    
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFootstepTime >= FOOTSTEP_INTERVAL) {
        
        if (useCustomFootsteps && !customFootstepSounds.empty()) {
            playCustomFootstep();
        } else {
            // 기본 발자국 시스템
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> dis(0, 1);
            
            if (dis(gen) == 0) {
                playSound(SoundType::ECHO_FOOTSTEP); // 리버브 구두
            } else {
                playSound(SoundType::FOOTSTEP_STONE); // 일반 구두
            }
        }
        
        lastFootstepTime = currentTime;
    }
}

void AudioManager::playCustomFootstep() {
    if (customFootstepSounds.empty()) return;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, customFootstepSounds.size() - 1);
    
    SoundType selectedSound = customFootstepSounds[dis(gen)];
    playSound(selectedSound);
}

void AudioManager::addCustomFootstepSound(SoundType soundType) {
    // 중복 체크
    auto it = std::find(customFootstepSounds.begin(), customFootstepSounds.end(), soundType);
    if (it == customFootstepSounds.end()) {
        customFootstepSounds.push_back(soundType);
    }
}

std::vector<std::string> AudioManager::getLoadedSoundNames() const {
    std::vector<std::string> names;
    for (const auto& pair : namedSounds) {
        names.push_back(pair.first);
    }
    return names;
}

bool AudioManager::isSoundLoaded(const std::string& soundName) const {
    return namedSounds.find(soundName) != namedSounds.end();
}

bool AudioManager::isSoundLoaded(SoundType soundType) const {
    return sounds.find(soundType) != sounds.end();
}

void AudioManager::playMusic(const std::string& musicFile) {
    if (!initialized) return;
    
    stopMusic();
    
    backgroundMusic = Mix_LoadMUS(musicFile.c_str());
    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1); // 무한 반복
    } else {
        std::cerr << "Failed to load music: " << musicFile << " - " << Mix_GetError() << std::endl;
    }
}

void AudioManager::stopMusic() {
    if (backgroundMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
}

void AudioManager::setMasterVolume(int volume) {
    masterVolume = std::clamp(volume, 0, 100);
    
    // 전체 볼륨 적용
    setSFXVolume(sfxVolume);
    setMusicVolume(musicVolume);
}

void AudioManager::setSFXVolume(int volume) {
    sfxVolume = std::clamp(volume, 0, 100);
    
    // 마스터 볼륨 적용
    int effectiveVolume = (sfxVolume * masterVolume) / 100;
    int sdlVolume = (effectiveVolume * MIX_MAX_VOLUME) / 100;
    
    // 모든 사운드 청크에 볼륨 설정
    for (auto& pair : sounds) {
        if (pair.second) {
            Mix_VolumeChunk(pair.second, sdlVolume);
        }
    }
    
    for (auto& pair : namedSounds) {
        if (pair.second) {
            Mix_VolumeChunk(pair.second, sdlVolume);
        }
    }
}

void AudioManager::setMusicVolume(int volume) {
    musicVolume = std::clamp(volume, 0, 100);
    
    // 마스터 볼륨 적용
    int effectiveVolume = (musicVolume * masterVolume) / 100;
    int sdlVolume = (effectiveVolume * MIX_MAX_VOLUME) / 100;
    
    Mix_VolumeMusic(sdlVolume);
}

// ===== 유틸리티 함수들 =====

std::string AudioManager::getFileExtension(const std::string& filePath) const {
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filePath.substr(pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

bool AudioManager::isValidAudioFile(const std::string& filePath) const {
    std::string ext = getFileExtension(filePath);
    return ext == "wav" || ext == "mp3" || ext == "ogg" || ext == "flac" || ext == "aiff";
}

std::string AudioManager::getFileNameWithoutExtension(const std::string& filePath) const {
    namespace fs = std::filesystem;
    return fs::path(filePath).stem().string();
}

// ===== 절차적 사운드 생성 함수들 (기존 코드 유지) =====

Mix_Chunk* AudioManager::generateShoeClickSound() {
    const int sampleRate = 44100;
    const int duration = 120; // 120ms (짧고 깔끔하게)
    const int samples = (sampleRate * duration) / 1000;
    
    Uint16* audioBuffer = new Uint16[samples * 2]; // 스테레오
    
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float sample = 0.0f;
        
        // 힐 클릭 (고주파 클릭)
        float heelClick = sin(2.0f * M_PI * 2800.0f * t) * exp(-t * 40.0f);
        sample += heelClick * 0.6f;
        
        // 가죽 크리즈 (중간 주파수)
        float leatherCrease = sin(2.0f * M_PI * 800.0f * t) * exp(-t * 25.0f);
        sample += leatherCrease * 0.3f;
        
        // 바닥 임팩트 (저주파)
        float impact = sin(2.0f * M_PI * 150.0f * t) * exp(-t * 15.0f);
        sample += impact * 0.4f;
        
        // 또각 하모닉 (특징적인 구두 소리)
        float harmonic = sin(2.0f * M_PI * 1400.0f * t) * exp(-t * 30.0f);
        sample += harmonic * 0.2f;
        
        // 빠른 어택, 빠른 릴리즈 (구두의 특징)
        float envelope = 1.0f;
        if (i < samples * 0.02f) {
            envelope = static_cast<float>(i) / (samples * 0.02f); // 매우 빠른 어택
        } else {
            envelope = exp(-t * 12.0f); // 빠른 감쇠
        }
        
        sample *= envelope * 0.7f;
        
        // 16비트 변환
        Sint16 audioSample = static_cast<Sint16>(std::clamp(sample * 32767, -32767.0f, 32767.0f));
        audioBuffer[i * 2] = audioSample;     // 왼쪽 채널
        audioBuffer[i * 2 + 1] = audioSample; // 오른쪽 채널
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(audioBuffer), samples * 4);
    return chunk;
}

Mix_Chunk* AudioManager::generateFlashlightSound() {
    const int sampleRate = 44100;
    const int duration = 120; // 120ms
    const int samples = (sampleRate * duration) / 1000;
    
    Uint16* audioBuffer = new Uint16[samples * 2];
    
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        
        // 클릭 사운드 (높은 주파수에서 낮은 주파수로)
        float frequency = 1200.0f * (1.0f - t * 3.0f);
        if (frequency < 300.0f) frequency = 300.0f;
        
        float sample = sin(2.0f * M_PI * frequency * t);
        
        // 메탈릭 클릭 하모닉 추가
        sample += sin(2.0f * M_PI * frequency * 2.0f * t) * 0.3f;
        
        // 빠른 페이드 아웃
        float envelope = 1.0f - (t * 5.0f);
        if (envelope < 0) envelope = 0;
        
        sample *= envelope * 0.4f;
        
        Sint16 audioSample = static_cast<Sint16>(sample * 32767);
        audioBuffer[i * 2] = audioSample;
        audioBuffer[i * 2 + 1] = audioSample;
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(audioBuffer), samples * 4);
    return chunk;
}

Mix_Chunk* AudioManager::generateUIBeepSound() {
    const int sampleRate = 44100;
    const int duration = 100; // 100ms
    const int samples = (sampleRate * duration) / 1000;
    
    Uint16* audioBuffer = new Uint16[samples * 2];
    
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        
        // 깔끔한 비프음
        float sample = sin(2.0f * M_PI * 800.0f * t);
        
        // 부드러운 엔벨로프
        float envelope = sin(M_PI * t / (duration / 1000.0f));
        
        sample *= envelope * 0.25f;
        
        Sint16 audioSample = static_cast<Sint16>(sample * 32767);
        audioBuffer[i * 2] = audioSample;
        audioBuffer[i * 2 + 1] = audioSample;
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(audioBuffer), samples * 4);
    return chunk;
}

Mix_Chunk* AudioManager::generateReverbShoeSound() {
    const int sampleRate = 44100;
    const int duration = 2000; // 2초 (강한 리버브)
    const int samples = (sampleRate * duration) / 1000;
    
    Uint16* audioBuffer = new Uint16[samples * 2];
    
    // 강한 리버브 파라미터
    const float reverbDecay = 0.85f;     // 높은 감쇠율 (더 긴 리버브)
    const float reverbWetness = 0.7f;    // 리버브 강도
    const int numReflections = 15;       // 많은 반사음
    const float roomSize = 1.2f;         // 큰 동굴
    
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float sample = 0.0f;
        
        // 원본 구두 소리 (첫 120ms)
        if (i < sampleRate * 0.12f) {
            float localT = static_cast<float>(i) / (sampleRate * 0.12f);
            
            // 힐 클릭 (또각 소리의 핵심)
            float heelClick = sin(2.0f * M_PI * 2800.0f * localT) * exp(-localT * 40.0f);
            
            // 가죽 소리
            float leather = sin(2.0f * M_PI * 800.0f * localT) * exp(-localT * 25.0f);
            
            // 바닥 임팩트
            float impact = sin(2.0f * M_PI * 150.0f * localT) * exp(-localT * 15.0f);
            
            // 하모닉
            float harmonic = sin(2.0f * M_PI * 1400.0f * localT) * exp(-localT * 30.0f);
            
            float drySignal = (heelClick * 0.6f + leather * 0.3f + impact * 0.4f + harmonic * 0.2f);
            
            // 빠른 어택
            float envelope = 1.0f;
            if (localT < 0.02f) {
                envelope = localT / 0.02f;
            } else {
                envelope = exp(-localT * 12.0f);
            }
            
            sample += drySignal * envelope * 0.5f; // 원본 소리 (드라이)
        }
        
        // 강한 리버브 (다중 반사음)
        for (int reflection = 1; reflection <= numReflections; reflection++) {
            float reflectionDelay = reflection * roomSize * 0.05f + 
                                  sin(reflection * 0.7f) * 0.02f; // 불규칙한 반사
            int reflectionStartSample = static_cast<int>(reflectionDelay * sampleRate);
            
            if (i >= reflectionStartSample) {
                float reflectionAge = (i - reflectionStartSample) / static_cast<float>(sampleRate);
                
                // 반사음 볼륨 (거리에 따라 감쇠)
                float reflectionVolume = pow(reverbDecay, reflection) * reverbWetness * 
                                       exp(-reflectionAge * 2.0f);
                
                if (reflectionVolume > 0.001f) { // 최소 임계값
                    // 반사음 생성 (원본의 변형)
                    float reflectionSample = 0.0f;
                    
                    // 고주파 감쇠 (거리감)
                    float highFreqLoss = pow(0.7f, reflection);
                    
                    // 힐 클릭 반사
                    float reflectedClick = sin(2.0f * M_PI * 2800.0f * highFreqLoss * reflectionAge) * 
                                         exp(-reflectionAge * 20.0f);
                    
                    // 저주파 반사 (동굴 울림)
                    float reflectedRumble = sin(2.0f * M_PI * 200.0f * reflectionAge) * 
                                          exp(-reflectionAge * 8.0f);
                    
                    reflectionSample = (reflectedClick * 0.4f + reflectedRumble * 0.6f) * 
                                     reflectionVolume;
                    
                    sample += reflectionSample;
                }
            }
        }
        
        // 디퓨즈 리버브 테일 (배경 잔향)
        if (t > 0.15f) {
            float tailDecay = exp(-(t - 0.15f) * 1.2f);
            float diffuseReverb = sin(2.0f * M_PI * 300.0f * t + 
                                    sin(t * 15.0f) * 0.5f) * tailDecay * 0.08f;
            sample += diffuseReverb;
        }
        
        // 16비트 변환 (클리핑 방지)
        Sint16 audioSample = static_cast<Sint16>(std::clamp(sample * 32767, -32767.0f, 32767.0f));
        
        // 스테레오 처리 (더 넓은 스테레오 필드)
        float stereoSpread = sin(t * 2.0f * M_PI * 0.4f) * 0.15f;
        Sint16 leftSample = static_cast<Sint16>(audioSample * (1.0f - stereoSpread));
        Sint16 rightSample = static_cast<Sint16>(audioSample * (1.0f + stereoSpread));
        
        audioBuffer[i * 2] = leftSample;      // 왼쪽 채널
        audioBuffer[i * 2 + 1] = rightSample; // 오른쪽 채널
    }
    
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(audioBuffer), samples * 4);
    return chunk;
}

// 이전 함수들 (호환성 유지)
Mix_Chunk* AudioManager::generateFootstepSound() {
    return generateShoeClickSound();
}

Mix_Chunk* AudioManager::generateEchoFootstepSound() {
    return generateReverbShoeSound();
}

Mix_Chunk* AudioManager::generateReverbFootstepSound() {
    return generateReverbShoeSound();
}
