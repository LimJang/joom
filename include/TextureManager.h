#pragma once
#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <vector>

class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer, const std::string& textureDir);
    ~TextureManager();

    void cleanup();

    bool loadTexture(const std::string& name, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& name);
    Uint32 sampleTexture(const std::string& name, float u, float v);
    const std::vector<Uint32>* getPixels(const std::string& name, int& width, int& height);

    // 절차적 텍스처 생성
    bool createWallTexture(const std::string& name, int width, int height, int type);
    bool createFloorTexture(const std::string& name, int width, int height);
    bool createCeilingTexture(const std::string& name, int width, int height);

private:
    bool cacheTexturePixels(const std::string& name, SDL_Texture* texture);

    SDL_Renderer* renderer;
    std::map<std::string, SDL_Texture*> textures;
    std::string textureDirectory;

    // 픽셀 데이터 캐시
    std::map<std::string, std::vector<Uint32>> texturePixels;
    std::map<std::string, int> textureWidths;
    std::map<std::string, int> textureHeights;
};