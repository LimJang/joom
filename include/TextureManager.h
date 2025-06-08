#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

class TextureManager {
private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textures;
    
public:
    TextureManager(SDL_Renderer* sdlRenderer);
    ~TextureManager();
    
    // 텍스처 로드
    bool loadTexture(const std::string& id, const std::string& filepath);
    
    // 프로시저럴 텍스처 생성 (파일 없이 코드로 생성)
    bool createWallTexture(const std::string& id, int width, int height, int type);
    bool createFloorTexture(const std::string& id, int width, int height);
    
    // 텍스처 가져오기
    SDL_Texture* getTexture(const std::string& id);
    
    // 텍스처 정리
    void cleanup();
    
    // 텍스처에서 특정 픽셀 색상 가져오기 (간단한 샘플링)
    Uint32 sampleTexture(const std::string& id, float u, float v);
};
