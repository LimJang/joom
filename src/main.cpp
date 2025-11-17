#include "Game.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <SDL2/SDL.h>

// 프로젝트 루트에서 리소스 폴더를 찾는 함수
std::string findResourcePath() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    char* basePath_c = SDL_GetBasePath();
    if (basePath_c) {
        currentPath = std::filesystem::path(basePath_c);
        SDL_free(basePath_c);
    }

    // "Joom" 디렉토리를 찾을 때까지 상위로 이동
    while (!currentPath.empty()) {
        if (std::filesystem::exists(currentPath / "textures") && std::filesystem::exists(currentPath / "sounds")) {
            return currentPath.string() + "/";
        }
        if (currentPath.has_parent_path()) {
            currentPath = currentPath.parent_path();
        } else {
            break;
        }
    }
    
    // 찾지 못했을 경우 현재 작업 디렉토리를 기본값으로 사용
    return "./";
}


int main() {
    // 리소스 경로 설정
    std::string resourcePath = findResourcePath();
    std::cout << "Resource path set to: " << resourcePath << std::endl;

    Game game;
    
    if (!game.initialize(resourcePath)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}
