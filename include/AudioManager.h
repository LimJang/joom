#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include <map>

enum class SoundType {
    UI_BEEP,
    FLASHLIGHT_TOGGLE,
    FOOTSTEP_STONE,
    ECHO_FOOTSTEP
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void cleanup();

    // Sound Loading
    bool loadSoundFile(const std::string& filePath, SoundType soundType);
    bool loadSoundFile(const std::string& filePath, const std::string& soundName);
    bool loadSoundsFromDirectory(const std::string& directoryPath);

    // Sound Playback
    void playSound(SoundType soundType);
    void playSound(const std::string& soundName);
    void playMusic(const std::string& musicFile);
    void stopMusic();
    void playFootstep();

    // Volume Control
    void setMasterVolume(int volume);
    void setSFXVolume(int volume);
    void setMusicVolume(int volume);
    void increaseMasterVolume();
    void decreaseMasterVolume();
    int getMasterVolume() const;

    // State & Info
    bool isInitialized() const { return initialized; }
    bool isSoundLoaded(const std::string& soundName) const;
    bool isSoundLoaded(SoundType soundType) const;
    std::vector<std::string> getLoadedSoundNames() const;
    
    // Positional Audio
    void updatePositionalSound(const std::string& soundName, float distance, float maxDistance);
    void stopPositionalSound(const std::string& soundName);

private:
    void generateSounds();
    Mix_Chunk* generateSimpleSound(int frequency, int duration);

    // Utility
    std::string getFileExtension(const std::string& filePath) const;
    bool isValidAudioFile(const std::string& filePath) const;
    std::string getFileNameWithoutExtension(const std::string& filePath) const;

    // Compatibility
    Mix_Chunk* generateShoeClickSound();
    Mix_Chunk* generateFlashlightSound();
    Mix_Chunk* generateUIBeepSound();
    Mix_Chunk* generateReverbShoeSound();
    Mix_Chunk* generateFootstepSound();
    Mix_Chunk* generateEchoFootstepSound();
    Mix_Chunk* generateReverbFootstepSound();

    bool initialized;
    int masterVolume; // 0-100
    int sfxVolume;    // 0-100
    int musicVolume;  // 0-100

    Mix_Music* backgroundMusic;
    std::map<SoundType, Mix_Chunk*> sounds;
    std::map<std::string, Mix_Chunk*> namedSounds;
    std::map<std::string, int> positionalSoundChannels;

    // Footstep logic
    Uint32 lastFootstepTime;
    bool useCustomFootsteps;
    bool nextFootstepIsLeft;
    std::vector<SoundType> customFootstepSounds;
    const int FOOTSTEP_INTERVAL = 400;
};
