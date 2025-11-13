#pragma once
#include <SDL2/SDL.h>
#include <map>
#include <string>

class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer, const std::string& textureDir);
    ~TextureManager();

    void cleanup();

    bool loadTexture(const std::string& name, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& name);
    Uint32 sampleTexture(const std::string& id, float u, float v);

    // 절차적 텍스처 생성
    bool createWallTexture(const std::string& name, int width, int height, int type);
    bool createFloorTexture(const std::string& name, int width, int height);

private:
    SDL_Renderer* renderer;
    std::map<std::string, SDL_Texture*> textures;
    std::string textureDirectory;
};