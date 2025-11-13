#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

// .cpp 파일에 정의된 모든 SoundType을 포함하도록 수정
enum class SoundType {
    UI_BEEP,
    FLASHLIGHT_TOGGLE,
    FOOTSTEP_DEFAULT,
    CUSTOM_FOOTSTEP_1,
    CUSTOM_FOOTSTEP_2,
    FOOTSTEP_STONE,
    ECHO_FOOTSTEP
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void cleanup();

    // 파일 로딩
    bool loadSoundFile(const std::string& filePath, SoundType soundType);
    bool loadSoundFile(const std::string& filePath, const std::string& soundName);
    bool loadSoundsFromDirectory(const std::string& directoryPath);

    // 사운드 재생
    void playSound(SoundType soundType);
    void playSound(const std::string& soundName);
    void playMusic(const std::string& musicFile);
    void stopMusic();

    // 위치 기반 사운드 제어
    void updatePositionalSound(const std::string& soundName, float distance, float maxDistance);
    void stopPositionalSound(const std::string& soundName);

    // 발자국 소리
    void playFootstep();
    void addCustomFootstepSound(SoundType soundType);
    void enableCustomFootsteps(bool enable) { useCustomFootsteps = enable; }
    bool isUsingCustomFootsteps() const { return useCustomFootsteps; }

    // 볼륨 제어
    void setMasterVolume(int volume);
    void setSFXVolume(int volume);
    void setMusicVolume(int volume);

    // 상태 및 정보 GET
    bool isInitialized() const { return initialized; }
    bool isSoundLoaded(const std::string& soundName) const;
    bool isSoundLoaded(SoundType soundType) const;
    std::vector<std::string> getLoadedSoundNames() const;
    int getMasterVolume() const;

private:
    // 절차적 사운드 생성
    void generateSounds();
    Mix_Chunk* generateSimpleSound(int frequency, int duration);
    
    // 호환성을 위한 이전 함수들
    Mix_Chunk* generateShoeClickSound();
    Mix_Chunk* generateFlashlightSound();
    Mix_Chunk* generateUIBeepSound();
    Mix_Chunk* generateReverbShoeSound();
    Mix_Chunk* generateFootstepSound();
    Mix_Chunk* generateEchoFootstepSound();
    Mix_Chunk* generateReverbFootstepSound();

    // 유틸리티
    std::string getFileExtension(const std::string& filePath) const;
    bool isValidAudioFile(const std::string& filePath) const;
    std::string getFileNameWithoutExtension(const std::string& filePath) const;
    void playCustomFootstep();

    bool initialized;
    int masterVolume;
    int sfxVolume;
    int musicVolume;

    // 사운드/음악 데이터
    std::map<SoundType, Mix_Chunk*> sounds;
    std::map<std::string, Mix_Chunk*> namedSounds;
    Mix_Music* backgroundMusic;

    // 발자국 관련
    std::vector<SoundType> customFootstepSounds;
    bool useCustomFootsteps;
    Uint32 lastFootstepTime;
    static constexpr Uint32 FOOTSTEP_INTERVAL = 350; // ms

    // 위치 기반 사운드 채널 추적
    std::map<std::string, int> positionalSoundChannels;
};