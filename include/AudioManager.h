#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <vector>

enum class SoundType {
    FOOTSTEP_STONE,
    FLASHLIGHT_TOGGLE,
    AMBIENT_CAVE,
    UI_BEEP,
    ECHO_FOOTSTEP,
    // 커스텀 사운드 슬롯들
    CUSTOM_FOOTSTEP_1,
    CUSTOM_FOOTSTEP_2,
    CUSTOM_AMBIENT_1,
    CUSTOM_AMBIENT_2,
    CUSTOM_UI_1,
    CUSTOM_WEAPON_1,
    CUSTOM_DOOR_1,
    CUSTOM_PICKUP_1
};

class AudioManager {
private:
    std::unordered_map<SoundType, Mix_Chunk*> sounds;
    std::unordered_map<std::string, Mix_Chunk*> namedSounds; // 이름으로 접근 가능한 사운드
    Mix_Music* backgroundMusic;
    
    bool initialized;
    int masterVolume;
    int sfxVolume;
    int musicVolume;
    
    // 발자국 타이밍 제어
    Uint32 lastFootstepTime;
    const Uint32 FOOTSTEP_INTERVAL = 300;
    
    // 커스텀 사운드 설정
    bool useCustomFootsteps;
    std::vector<SoundType> customFootstepSounds;
    
public:
    AudioManager();
    ~AudioManager();
    
    bool initialize();
    void cleanup();
    
    // 사운드 생성 및 로딩
    void generateSounds();
    bool loadSoundFile(const std::string& filePath, SoundType soundType);
    bool loadSoundFile(const std::string& filePath, const std::string& soundName);
    bool loadSoundsFromDirectory(const std::string& directoryPath);
    
    // 사운드 재생
    void playSound(SoundType soundType);
    void playSound(const std::string& soundName);
    void playFootstep();
    void playCustomFootstep(); // 커스텀 발자국 재생
    void playMusic(const std::string& musicFile);
    void stopMusic();
    
    // 커스텀 사운드 관리
    void enableCustomFootsteps(bool enable) { useCustomFootsteps = enable; }
    void addCustomFootstepSound(SoundType soundType);
    void clearCustomFootstepSounds() { customFootstepSounds.clear(); }
    
    // 사운드 정보
    std::vector<std::string> getLoadedSoundNames() const;
    bool isSoundLoaded(const std::string& soundName) const;
    bool isSoundLoaded(SoundType soundType) const;
    
    // 볼륨 제어
    void setMasterVolume(int volume);
    void setSFXVolume(int volume);
    void setMusicVolume(int volume);
    
    // 상태 확인
    bool isInitialized() const { return initialized; }
    int getMasterVolume() const { return masterVolume; }
    int getSFXVolume() const { return sfxVolume; }
    int getMusicVolume() const { return musicVolume; }
    bool isUsingCustomFootsteps() const { return useCustomFootsteps; }
    
private:
    // 절차적 사운드 생성 함수들
    Mix_Chunk* generateShoeClickSound();
    Mix_Chunk* generateReverbShoeSound();
    Mix_Chunk* generateFlashlightSound();
    Mix_Chunk* generateUIBeepSound();
    
    // 이전 함수들 (호환성)
    Mix_Chunk* generateFootstepSound();
    Mix_Chunk* generateEchoFootstepSound(); 
    Mix_Chunk* generateReverbFootstepSound();
    
    // 파일 유틸리티
    std::string getFileExtension(const std::string& filePath) const;
    bool isValidAudioFile(const std::string& filePath) const;
    std::string getFileNameWithoutExtension(const std::string& filePath) const;
};
