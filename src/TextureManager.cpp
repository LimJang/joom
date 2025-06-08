#include "TextureManager.h"
#include <iostream>
#include <cmath>

TextureManager::TextureManager(SDL_Renderer* sdlRenderer) : renderer(sdlRenderer) {
}

TextureManager::~TextureManager() {
    cleanup();
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filepath) {
    SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << filepath << " - " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "Failed to create texture from surface - " << SDL_GetError() << std::endl;
        return false;
    }
    
    textures[id] = texture;
    return true;
}

bool TextureManager::createWallTexture(const std::string& id, int width, int height, int type) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                           SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        std::cerr << "Failed to create wall texture - " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 텍스처를 렌더 타겟으로 설정
    SDL_SetRenderTarget(renderer, texture);
    
    // 텍스처 패턴 생성
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
                
                // 벽돌 사이 선
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
                
                // 무작위 돌 패턴
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
                
                // 수직 금속 판 패턴
                SDL_SetRenderDrawColor(renderer, 90, 90, 100, 255);
                for (int x = 8; x < width; x += 16) {
                    SDL_RenderDrawLine(renderer, x, 0, x, height);
                }
                
                // 수평 리벳
                SDL_SetRenderDrawColor(renderer, 110, 110, 120, 255);
                for (int y = 8; y < height; y += 24) {
                    for (int x = 4; x < width; x += 16) {
                        SDL_Rect rivet = {x, y, 2, 2};
                        SDL_RenderFillRect(renderer, &rivet);
                    }
                }
            }
            break;
            
        default: // 기본 패턴
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderClear(renderer);
            break;
    }
    
    // 렌더 타겟을 다시 화면으로 복원
    SDL_SetRenderTarget(renderer, nullptr);
    
    textures[id] = texture;
    return true;
}

bool TextureManager::createFloorTexture(const std::string& id, int width, int height) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                           SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        std::cerr << "Failed to create floor texture - " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_SetRenderTarget(renderer, texture);
    
    // 체크무늬 바닥 패턴
    for (int y = 0; y < height; y += 16) {
        for (int x = 0; x < width; x += 16) {
            if ((x/16 + y/16) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255); // 어두운 회색
            } else {
                SDL_SetRenderDrawColor(renderer, 96, 96, 96, 255); // 밝은 회색
            }
            
            SDL_Rect tile = {x, y, 16, 16};
            SDL_RenderFillRect(renderer, &tile);
        }
    }
    
    SDL_SetRenderTarget(renderer, nullptr);
    
    textures[id] = texture;
    return true;
}

SDL_Texture* TextureManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void TextureManager::cleanup() {
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}

Uint32 TextureManager::sampleTexture(const std::string& id, float u, float v) {
    // 간단한 텍스처 샘플링 (실제 구현은 복잡하지만 여기서는 기본 색상 반환)
    if (id == "wall_brick") return 0xFF8B4513; // 갈색
    if (id == "wall_stone") return 0xFF696969; // 회색
    if (id == "wall_metal") return 0xFF464650; // 어두운 금속색
    if (id == "floor") return 0xFF404040; // 어두운 회색
    
    return 0xFFFFFFFF; // 기본 흰색
}
