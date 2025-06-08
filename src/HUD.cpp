#include "HUD.h"
#include <sstream>
#include <iomanip>
#include <cstring>

HUD::HUD(SDL_Renderer* sdlRenderer, int width, int height) 
    : renderer(sdlRenderer), screenWidth(width), screenHeight(height),
      health(100), maxHealth(100), ammo(50), maxAmmo(200), score(0), fps(60.0f), flashlightOn(true) {
}

HUD::~HUD() {
}

void HUD::render() {
    // HUD 요소들 렌더링
    renderHealthBar();
    renderAmmoCounter();
    renderScore();
    renderFPS();
    renderCrosshair();
    renderFlashlightStatus();
}

void HUD::renderHealthBar() {
    int barWidth = 200;
    int barHeight = 20;
    int x = 20;
    int y = screenHeight - 60;
    
    // 배경 (검은색 테두리)
    drawRect(x - 2, y - 2, barWidth + 4, barHeight + 4, 0, 0, 0);
    
    // 체력바 배경 (어두운 빨간색)
    drawFilledRect(x, y, barWidth, barHeight, 64, 0, 0);
    
    // 현재 체력 (밝은 빨간색에서 초록색으로 그라데이션)
    float healthRatio = (float)health / maxHealth;
    int currentWidth = (int)(barWidth * healthRatio);
    
    Uint8 red = health < 30 ? 255 : (Uint8)(255 * (1.0f - (healthRatio - 0.3f) / 0.7f));
    Uint8 green = health < 30 ? 0 : (Uint8)(255 * ((healthRatio - 0.3f) / 0.7f));
    
    if (currentWidth > 0) {
        drawFilledRect(x, y, currentWidth, barHeight, red, green, 0);
    }
    
    // 체력 수치 표시
    renderText("HEALTH", x, y - 15);
    renderNumber(health, x + barWidth - 60, y + 2);
}

void HUD::renderAmmoCounter() {
    int x = screenWidth - 150;
    int y = screenHeight - 60;
    
    // 탄약 텍스트와 숫자
    renderText("AMMO", x, y - 15);
    
    // 현재 탄약
    renderNumber(ammo, x, y, 2);
    
    // 슬래시
    renderText("/", x + 60, y + 5);
    
    // 최대 탄약
    renderNumber(maxAmmo, x + 80, y + 5, 1);
}

void HUD::renderScore() {
    int x = (screenWidth - 100) / 2;
    int y = 20;
    
    renderText("SCORE", x, y);
    renderNumber(score, x, y + 20, 2);
}

void HUD::renderFPS() {
    int x = screenWidth - 80;
    int y = 20;
    
    renderText("FPS", x, y);
    renderNumber((int)fps, x, y + 15);
}

void HUD::renderCrosshair() {
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    int size = 10;
    
    // 십자가 조준점
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 흰색
    
    // 수직선
    SDL_RenderDrawLine(renderer, centerX, centerY - size, centerX, centerY + size);
    // 수평선
    SDL_RenderDrawLine(renderer, centerX - size, centerY, centerX + size, centerY);
    
    // 중앙 점
    SDL_Rect center = {centerX - 1, centerY - 1, 2, 2};
    SDL_RenderFillRect(renderer, &center);
}

void HUD::renderFlashlightStatus() {
    int x = 20;
    int y = 20;
    
    // 손전등 아이콘 (간단한 방형)
    if (flashlightOn) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 노란색 (켜짐)
        renderText("LIGHT ON", x, y);
        
        // 빛 아이콘
        SDL_Rect lightIcon = {x + 80, y + 2, 12, 8};
        SDL_RenderFillRect(renderer, &lightIcon);
        
        // 빛 빔기
        for (int i = 0; i < 3; i++) {
            SDL_RenderDrawLine(renderer, x + 92 + i * 4, y + 6, x + 96 + i * 4, y + 6);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // 회색 (꺼짐)
        renderText("LIGHT OFF", x, y);
        
        // 꺼진 아이콘
        SDL_Rect lightIcon = {x + 80, y + 2, 12, 8};
        SDL_RenderDrawRect(renderer, &lightIcon);
    }
    
    // 설명 텍스트
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    renderText("F: TOGGLE", x, y + 15, 1);
}

void HUD::renderNumber(int number, int x, int y, int scale) {
    std::string numStr = std::to_string(number);
    
    for (size_t i = 0; i < numStr.length(); i++) {
        char digit = numStr[i];
        int digitValue = digit - '0';
        
        // 간단한 7세그먼트 디스플레이 스타일 숫자
        renderDigit(digitValue, x + i * (8 * scale), y, scale);
    }
}

void HUD::renderDigit(int digit, int x, int y, int scale) {
    // 7세그먼트 디스플레이 패턴
    bool segments[10][7] = {
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}  // 9
    };
    
    if (digit < 0 || digit > 9) return;
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // 초록색 (레트로 스타일)
    
    int w = 2 * scale; // 세그먼트 두께
    int len = 6 * scale; // 세그먼트 길이
    
    // 세그먼트 위치 정의
    if (segments[digit][0]) { // 상단
        SDL_Rect seg = {x, y, len, w};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][1]) { // 우상단
        SDL_Rect seg = {x + len - w, y, w, len};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][2]) { // 우하단
        SDL_Rect seg = {x + len - w, y + len, w, len};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][3]) { // 하단
        SDL_Rect seg = {x, y + 2 * len - w, len, w};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][4]) { // 좌하단
        SDL_Rect seg = {x, y + len, w, len};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][5]) { // 좌상단
        SDL_Rect seg = {x, y, w, len};
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segments[digit][6]) { // 중간
        SDL_Rect seg = {x, y + len - w/2, len, w};
        SDL_RenderFillRect(renderer, &seg);
    }
}

void HUD::renderText(const std::string& text, int x, int y, int scale) {
    // 간단한 비트맵 스타일 텍스트 (픽셀 폰트)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 노란색
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        renderChar(c, x + i * (6 * scale), y, scale);
    }
}

void HUD::renderChar(char c, int x, int y, int scale) {
    // 간단한 비트맵 스타일 텍스트 (일부 문자만 구현)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 노란색
    
    // 간단한 5x7 폰트 패턴
    bool pattern[7][5] = {0}; // 기본값은 모두 false
    
    // 주요 문자들 구현
    if (c == 'H') {
        bool h_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}
        };
        memcpy(pattern, h_pattern, sizeof(pattern));
    }
    else if (c == 'E') {
        bool e_pattern[7][5] = {
            {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,0},
            {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}
        };
        memcpy(pattern, e_pattern, sizeof(pattern));
    }
    else if (c == 'A') {
        bool a_pattern[7][5] = {
            {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}
        };
        memcpy(pattern, a_pattern, sizeof(pattern));
    }
    else if (c == 'L') {
        bool l_pattern[7][5] = {
            {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
            {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}
        };
        memcpy(pattern, l_pattern, sizeof(pattern));
    }
    else if (c == 'T') {
        bool t_pattern[7][5] = {
            {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
            {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}
        };
        memcpy(pattern, t_pattern, sizeof(pattern));
    }
    else if (c == 'M') {
        bool m_pattern[7][5] = {
            {1,0,0,0,1}, {1,1,0,1,1}, {1,0,1,0,1}, {1,0,0,0,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}
        };
        memcpy(pattern, m_pattern, sizeof(pattern));
    }
    else if (c == 'O') {
        bool o_pattern[7][5] = {
            {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}
        };
        memcpy(pattern, o_pattern, sizeof(pattern));
    }
    else if (c == 'S') {
        bool s_pattern[7][5] = {
            {0,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {0,1,1,1,0},
            {0,0,0,0,1}, {0,0,0,0,1}, {1,1,1,1,0}
        };
        memcpy(pattern, s_pattern, sizeof(pattern));
    }
    else if (c == 'C') {
        bool c_pattern[7][5] = {
            {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0},
            {1,0,0,0,0}, {1,0,0,0,1}, {0,1,1,1,0}
        };
        memcpy(pattern, c_pattern, sizeof(pattern));
    }
    else if (c == 'R') {
        bool r_pattern[7][5] = {
            {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0},
            {1,0,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1}
        };
        memcpy(pattern, r_pattern, sizeof(pattern));
    }
    else if (c == 'F') {
        bool f_pattern[7][5] = {
            {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,0},
            {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}
        };
        memcpy(pattern, f_pattern, sizeof(pattern));
    }
    else if (c == 'P') {
        bool p_pattern[7][5] = {
            {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0},
            {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}
        };
        memcpy(pattern, p_pattern, sizeof(pattern));
    }
    else if (c == '/') {
        bool slash_pattern[7][5] = {
            {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0},
            {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}
        };
        memcpy(pattern, slash_pattern, sizeof(pattern));
    }
    else if (c >= '0' && c <= '9') {
        // 숫자에 대해서는 renderDigit 사용
        renderDigit(c - '0', x, y, scale);
        return;
    }
    else if (c == ' ') {
        // 공백은 아무것도 그리지 않음
        return;
    }
    else {
        // 정의되지 않은 문자는 사각형으로 표시
        SDL_Rect rect = {x, y, 4 * scale, 6 * scale};
        SDL_RenderDrawRect(renderer, &rect);
        return;
    }
    
    // 패턴을 이용해 문자 그리기
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (pattern[row][col]) {
                SDL_Rect pixel = {
                    x + col * scale, 
                    y + row * scale, 
                    scale, 
                    scale
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

void HUD::drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void HUD::drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}
