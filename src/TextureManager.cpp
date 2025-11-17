#include "TextureManager.h"
#include <SDL2/SDL_image.h>
#include <iostream>

TextureManager::TextureManager(SDL_Renderer* renderer, const std::string& textureDir)
    : renderer(renderer), textureDirectory(textureDir) {}

TextureManager::~TextureManager() {
    cleanup();
}

void TextureManager::cleanup() {
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    texturePixels.clear();
    textureWidths.clear();
    textureHeights.clear();
}

bool TextureManager::cacheTexturePixels(const std::string& name, SDL_Texture* texture) {
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) {
        std::cerr << "Failed to create surface for caching texture: " << SDL_GetError() << std::endl;
        return false;
    }

    // This is a workaround. Ideally, we'd get the pixel data directly.
    // For now, we render the texture to a surface to read its pixels.
    SDL_Texture* tempTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetRenderTarget(renderer, tempTarget);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_DestroyTexture(tempTarget);

    std::vector<Uint32> pixels(width * height);
    memcpy(pixels.data(), surface->pixels, surface->pitch * height);
    
    SDL_FreeSurface(surface);

    texturePixels[name] = pixels;
    textureWidths[name] = width;
    textureHeights[name] = height;

    return true;
}

bool TextureManager::loadTexture(const std::string& name, const std::string& filePath) {
    std::string fullPath = textureDirectory + filePath;
    SDL_Texture* texture = IMG_LoadTexture(renderer, fullPath.c_str());
    if (texture == nullptr) {
        std::cerr << "Failed to load texture: " << fullPath << " - " << IMG_GetError() << std::endl;
        return false;
    }
    
    if (textures.count(name)) {
        SDL_DestroyTexture(textures[name]);
    }
    textures[name] = texture;

    // 픽셀 데이터 캐시
    if (!cacheTexturePixels(name, texture)) {
        std::cerr << "Failed to cache pixels for texture: " << name << std::endl;
        return false;
    }

    return true;
}

bool TextureManager::createWallTexture(const std::string& id, int width, int height, int type) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                           SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        std::cerr << "Failed to create wall texture - " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_SetRenderTarget(renderer, texture);
    
    // ... (패턴 생성 코드는 동일)
    switch (type) {
        case 0: // 벽돌 패턴
            {
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // 갈색 베이스
                SDL_RenderClear(renderer);
                
                SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); // 밝은 갈색
                for (int y = 0; y < height; y += 16) {
                    for (int x = 0; x < width; x += 32) {
                        int offsetX = (y / 16) % 2 ? 16 : 0;
                        SDL_Rect brick = {x + offsetX, y, 30, 14};
                        SDL_RenderFillRect(renderer, &brick);
                    }
                }
                
                SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255); // 어두운 갈색
                for (int y = 0; y < height; y += 16) {
                    SDL_RenderDrawLine(renderer, 0, y, width, y);
                }
                for (int y = 0; y < height; y += 32) {
                    for (int x = 16; x < width; x += 32) {
                        int offsetX = (y / 16) % 2 ? 0 : 16;
                        SDL_RenderDrawLine(renderer, x + offsetX, y, x + offsetX, y + 16);
                    }
                }
            }
            break;
        case 1: // 돌 패턴
            {
                SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255); // 회색 베이스
                SDL_RenderClear(renderer);
                for (int i = 0; i < 100; i++) {
                    int x = (i * 17) % width;
                    int y = (i * 23) % height;
                    int size = 3 + (i % 5);
                    Uint8 brightness = 80 + (i % 50);
                    SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
                    SDL_Rect stone = {x, y, size, size};
                    SDL_RenderFillRect(renderer, &stone);
                }
            }
            break;
        case 2: // 금속 패턴
            {
                SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255); // 어두운 금속색
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 90, 90, 100, 255);
                for (int x = 8; x < width; x += 16) {
                    SDL_RenderDrawLine(renderer, x, 0, x, height);
                }
                SDL_SetRenderDrawColor(renderer, 110, 110, 120, 255);
                for (int y = 8; y < height; y += 24) {
                    for (int x = 4; x < width; x += 16) {
                        SDL_Rect rivet = {x, y, 2, 2};
                        SDL_RenderFillRect(renderer, &rivet);
                    }
                }
            }
            break;
        default:
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderClear(renderer);
            break;
    }
    
    SDL_SetRenderTarget(renderer, nullptr);
    
    textures[id] = texture;
    return cacheTexturePixels(id, texture);
}

bool TextureManager::createFloorTexture(const std::string& id, int width, int height) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                           SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        std::cerr << "Failed to create floor texture - " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_SetRenderTarget(renderer, texture);
    
    for (int y = 0; y < height; y += 16) {
        for (int x = 0; x < width; x += 16) {
            if ((x/16 + y/16) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 96, 96, 96, 255);
            }
            SDL_Rect tile = {x, y, 16, 16};
            SDL_RenderFillRect(renderer, &tile);
        }
    }
    
    SDL_SetRenderTarget(renderer, nullptr);
    
    textures[id] = texture;
    return cacheTexturePixels(id, texture);
}

bool TextureManager::createCeilingTexture(const std::string& id, int width, int height) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        std::cerr << "Failed to create ceiling texture - " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderTarget(renderer, texture);

    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    for (int i = 0; i < width; i += 16) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }

    SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
    for (int y = 0; y < height; y += 16) {
        for (int x = 0; x < width; x += 16) {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);

    textures[id] = texture;
    return cacheTexturePixels(id, texture);
}

SDL_Texture* TextureManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

const std::vector<Uint32>* TextureManager::getPixels(const std::string& name, int& width, int& height) {
    auto it = texturePixels.find(name);
    if (it == texturePixels.end()) {
        width = 0;
        height = 0;
        return nullptr;
    }
    width = textureWidths[name];
    height = textureHeights[name];
    return &it->second;
}

Uint32 TextureManager::sampleTexture(const std::string& name, float u, float v) {
    auto it = texturePixels.find(name);
    if (it == texturePixels.end()) {
        return 0xFFFFFFFF; // 흰색 에러 색상
    }

    const auto& pixels = it->second;
    int width = textureWidths[name];
    int height = textureHeights[name];

    int texX = static_cast<int>(u * width) % width;
    int texY = static_cast<int>(v * height) % height;
    if (texX < 0) texX += width;
    if (texY < 0) texY += height;

    return pixels[texY * width + texX];
}
