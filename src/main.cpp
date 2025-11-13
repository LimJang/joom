#include "Game.h"
#include <iostream>
#include <string>
#include <SDL2/SDL.h>

int main() {
    // 리소스 경로 설정
    std::string resourcePath = "";
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        resourcePath = std::string(basePath) + "../Resources/";
        SDL_free(basePath);
    } else {
        // 만약을 위한 폴백
        resourcePath = "./";
    }

    Game game;
    
    if (!game.initialize(resourcePath)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}
