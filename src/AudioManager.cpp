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
      lastFootstepTime(0), useCustomFootsteps(false), nextFootstepIsLeft(true) {
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
    positionalSoundChannels.clear();
}

void AudioManager::updatePositionalSound(const std::string& soundName, float distance, float maxDistance) {
    if (!initialized || namedSounds.find(soundName) == namedSounds.end()) {
        return;
    }

    auto it = positionalSoundChannels.find(soundName);

    if (distance < maxDistance) {
        int channel = -1;
        if (it == positionalSoundChannels.end()) {
            // 사운드 재생 시작
            channel = Mix_PlayChannel(-1, namedSounds[soundName], -1); // -1: 무한 반복
            if (channel != -1) {
                positionalSoundChannels[soundName] = channel;
            }
        } else {
            channel = it->second;
        }

        if (channel != -1) {
            // 거리에 따른 볼륨 계산 (선형 감쇠)
            float volumeRatio = 1.0f - (distance / maxDistance);
            int volume = static_cast<int>(MIX_MAX_VOLUME * std::clamp(volumeRatio, 0.0f, 1.0f));
            Mix_Volume(channel, volume);
        }
    } else {
        // 거리를 벗어났으면 사운드 정지
        if (it != positionalSoundChannels.end()) {
            stopPositionalSound(soundName);
        }
    }
}

void AudioManager::stopPositionalSound(const std::string& soundName) {
    if (!initialized) return;

    auto it = positionalSoundChannels.find(soundName);
    if (it != positionalSoundChannels.end()) {
        Mix_HaltChannel(it->second);
        positionalSoundChannels.erase(it);
    }
}


void AudioManager::generateSounds() {
    if (!initialized) return;
    
    // 간단한 사운드로 대체 (메모리 안전)
    sounds[SoundType::FOOTSTEP_STONE] = generateSimpleSound(800, 100);
    sounds[SoundType::FLASHLIGHT_TOGGLE] = generateSimpleSound(1200, 80);
    sounds[SoundType::UI_BEEP] = generateSimpleSound(1000, 150);
    sounds[SoundType::ECHO_FOOTSTEP] = generateSimpleSound(600, 200);
}

Mix_Chunk* AudioManager::generateSimpleSound(int frequency, int duration) {
    const int sampleRate = 44100;
    const int samples = (sampleRate * duration) / 1000;
    const int bufferSize = samples * 2 * sizeof(Sint16); // 스테레오 16비트
    
    // SDL_mixer에서 관리할 수 있도록 SDL_malloc 사용
    Uint8* audioBuffer = (Uint8*)SDL_malloc(bufferSize);
    if (!audioBuffer) {
        return nullptr;
    }
    
    Sint16* sampleBuffer = (Sint16*)audioBuffer;
    
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float envelope = 1.0f - (t / (duration / 1000.0f)); // 선형 페이드아웃
        float sample = sin(2.0f * M_PI * frequency * t) * envelope * 0.3f;
        
        Sint16 audioSample = static_cast<Sint16>(sample * 32767);
        sampleBuffer[i * 2] = audioSample;     // 왼쪽 채널
        sampleBuffer[i * 2 + 1] = audioSample; // 오른쪽 채널
    }
    
    Mix_Chunk* chunk = (Mix_Chunk*)SDL_malloc(sizeof(Mix_Chunk));
    if (!chunk) {
        SDL_free(audioBuffer);
        return nullptr;
    }
    
    chunk->allocated = 1;
    chunk->abuf = audioBuffer;
    chunk->alen = bufferSize;
    chunk->volume = MIX_MAX_VOLUME;
    
    return chunk;
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
        
        if (nextFootstepIsLeft) {
            if (isSoundLoaded("footstep1")) {
                playSound("footstep1");
            }
        } else {
            if (isSoundLoaded("footstep2")) {
                playSound("footstep2");
            }
        }
        nextFootstepIsLeft = !nextFootstepIsLeft;
        
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
        Mix_PlayMusic(backgroundMusic, -1);
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
    setSFXVolume(sfxVolume);
    setMusicVolume(musicVolume);
}

void AudioManager::setSFXVolume(int volume) {
    sfxVolume = std::clamp(volume, 0, 100);
    
    int effectiveVolume = (sfxVolume * masterVolume) / 100;
    int sdlVolume = (effectiveVolume * MIX_MAX_VOLUME) / 100;
    
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

// 호환성을 위한 이전 함수들
Mix_Chunk* AudioManager::generateShoeClickSound() {
    return generateSimpleSound(800, 100);
}

Mix_Chunk* AudioManager::generateFlashlightSound() {
    return generateSimpleSound(1200, 80);
}

Mix_Chunk* AudioManager::generateUIBeepSound() {
    return generateSimpleSound(1000, 150);
}

Mix_Chunk* AudioManager::generateReverbShoeSound() {
    return generateSimpleSound(600, 200);
}

Mix_Chunk* AudioManager::generateFootstepSound() {
    return generateSimpleSound(800, 100);
}

Mix_Chunk* AudioManager::generateEchoFootstepSound() {
    return generateSimpleSound(600, 200);
}

Mix_Chunk* AudioManager::generateReverbFootstepSound() {
    return generateSimpleSound(600, 200);
}

int AudioManager::getMasterVolume() const {
    return masterVolume;
}
