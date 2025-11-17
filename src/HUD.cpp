#include "HUD.h"
#include <sstream>
#include <iomanip>
#include <cstring>

HUD::HUD(SDL_Renderer* sdlRenderer, int width, int height) 
    : renderer(sdlRenderer), screenWidth(width), screenHeight(height),
      health(100), maxHealth(100), ammo(50), maxAmmo(200), fps(60.0f), 
      flashlightOn(true), audioEnabled(true), masterVolume(70),
      hasRedKey(false), hasBlueKey(false), hasYellowKey(false) {
}

HUD::~HUD() {
}

void HUD::render() {
    // HUD 요소들 렌더링
    renderHealthBar();
    renderAmmoCounter();
    renderFPS();
    renderCrosshair();
    renderFlashlightStatus();
    renderAudioStatus();
    renderKeyStatus();
    renderControls();
}

void HUD::renderLevelCompleteMessage(int level) {
    // 화면 중앙에 레벨 완료 메시지 표시
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    
    // 반투명 배경
    drawFilledRect(centerX - 150, centerY - 60, 300, 120, 0, 0, 0, 200);
    drawRect(centerX - 150, centerY - 60, 300, 120, 255, 255, 0);
    
    // 메시지 텍스트
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    renderText("LEVEL COMPLETE", centerX - 65, centerY - 40, 2);
    
    std::string levelText = "LEVEL " + std::to_string(level);
    renderText(levelText, centerX - 35, centerY - 10, 2);
    
    renderText("ADVANCING TO NEXT LEVEL", centerX - 100, centerY + 20, 1);
}

void HUD::renderKeyStatus() {
    int x = 20;
    int y = 80; // 오디오 상태 아래
    
    renderText("KEYS", x, y, 1);
    
    // 키 아이콘들 그리기
    drawKeyIcon(x + 35, y, hasRedKey, 255, 100, 100);    // 빨간 키
    drawKeyIcon(x + 55, y, hasBlueKey, 100, 150, 255);   // 파란 키
    drawKeyIcon(x + 75, y, hasYellowKey, 255, 255, 100); // 노란 키
}

void HUD::drawKeyIcon(int x, int y, bool hasKey, Uint8 r, Uint8 g, Uint8 b) {
    if (hasKey) {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255); // 어두운 회색
    }
    
    // 키 모양 그리기 (간단한 사각형 + 원형)
    SDL_Rect keyBody = {x, y + 2, 12, 4};
    SDL_RenderFillRect(renderer, &keyBody);
    
    SDL_Rect keyHead = {x + 12, y, 6, 8};
    SDL_RenderFillRect(renderer, &keyHead);
    
    // 키가 있으면 빛나는 효과
    if (hasKey) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
        SDL_Rect glow = {x - 1, y - 1, 20, 10};
        SDL_RenderDrawRect(renderer, &glow);
    }
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
        
        // 빛 빔
        for (int i = 0; i < 3; i++) {
            SDL_RenderDrawLine(renderer, x + 92 + i * 4, y + 6, x + 96 + i * 4, y + 6);
        }
    } else {
        // 손전등이 꺼져있으면 경고 표시
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 빨간색 (위험)
        renderText("TOTAL DARKNESS", x, y);
        renderText("PRESS F TO SURVIVE", x, y + 15);
        
        // 깜빡이는 경고 아이콘
        static int blinkTimer = 0;
        blinkTimer++;
        
        if ((blinkTimer / 30) % 2 == 0) { // 30프레임마다 깜빡
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
            SDL_Rect warningIcon = {x + 160, y + 2, 16, 12};
            SDL_RenderFillRect(renderer, &warningIcon);
            
            // 느낌표 (!)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, x + 168, y + 3, x + 168, y + 9);
            SDL_Rect dot = {x + 167, y + 11, 2, 2};
            SDL_RenderFillRect(renderer, &dot);
        }
    }
}

void HUD::renderAudioStatus() {
    int x = 20;
    int y = 50; // 손전등 상태 아래
    
    if (audioEnabled) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // 시아네색 (오디오 켜짐)
        renderText("AUDIO", x, y, 1);
        
        // 볼륨 바
        int barWidth = 50;
        int barHeight = 4;
        int barX = x + 50;
        int barY = y + 4;
        
        // 볼륨 배경
        drawRect(barX - 1, barY - 1, barWidth + 2, barHeight + 2, 255, 255, 255);
        drawFilledRect(barX, barY, barWidth, barHeight, 64, 64, 64);
        
        // 현재 볼륨
        int volumeWidth = (barWidth * masterVolume) / 100;
        if (volumeWidth > 0) {
            drawFilledRect(barX, barY, volumeWidth, barHeight, 0, 255, 255);
        }
        
        // 볼륨 수치
        renderNumber(masterVolume, barX + barWidth + 5, y, 1);
        
        // 스피커 아이콘
        SDL_Rect speaker = {x + 30, y + 2, 6, 6};
        SDL_RenderFillRect(renderer, &speaker);
        
        // 음파 표시
        for (int i = 1; i <= 3; i++) {
            if (masterVolume > i * 25) {
                SDL_RenderDrawLine(renderer, x + 37 + i * 2, y + 1, x + 37 + i * 2, y + 8);
            }
        }
        
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // 회색 (오디오 꺼짐)
        renderText("NO AUDIO", x, y, 1);
        
        // X 표시
        SDL_RenderDrawLine(renderer, x + 70, y + 2, x + 78, y + 10);
        SDL_RenderDrawLine(renderer, x + 78, y + 2, x + 70, y + 10);
    }
}

void HUD::renderControls() {
    int x = screenWidth - 200;
    int y = screenHeight - 140;
    
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // 연한 회색
    
    // 컨트롤 가이드 (키 수집 게임에 맞게 업데이트)
    renderText("CONTROLS", x, y, 1);
    renderText("WASD: MOVE", x, y + 15, 1);
    renderText("F: FLASHLIGHT", x, y + 25, 1);
    
    if (audioEnabled) {
        renderText("+/-: VOLUME", x, y + 35, 1);
    }
    
    renderText("ESC: EXIT", x, y + 45, 1);
}

// 기존의 renderNumber, renderDigit, renderText, renderChar, drawRect, drawFilledRect 함수들은 동일...
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
    
    // 주요 문자들 구현 (기존 문자들 + 새로운 문자들)
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
    else if (c == 'I') {
        bool i_pattern[7][5] = {
            {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
            {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1}
        };
        memcpy(pattern, i_pattern, sizeof(pattern));
    }
    else if (c == 'G') {
        bool g_pattern[7][5] = {
            {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,1,1,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}
        };
        memcpy(pattern, g_pattern, sizeof(pattern));
    }
    else if (c == 'N') {
        bool n_pattern[7][5] = {
            {1,0,0,0,1}, {1,1,0,0,1}, {1,0,1,0,1}, {1,0,0,1,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}
        };
        memcpy(pattern, n_pattern, sizeof(pattern));
    }
    else if (c == 'U') {
        bool u_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}
        };
        memcpy(pattern, u_pattern, sizeof(pattern));
    }
    else if (c == 'D') {
        bool d_pattern[7][5] = {
            {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
            {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0}
        };
        memcpy(pattern, d_pattern, sizeof(pattern));
    }
    else if (c == 'V') {
        bool v_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
            {1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0}
        };
        memcpy(pattern, v_pattern, sizeof(pattern));
    }
    else if (c == 'W') {
        bool w_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,1,0,1},
            {1,0,1,0,1}, {1,1,0,1,1}, {1,0,0,0,1}
        };
        memcpy(pattern, w_pattern, sizeof(pattern));
    }
    else if (c == 'X') {
        bool x_pattern[7][5] = {
            {1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0}, {0,0,1,0,0},
            {0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1}
        };
        memcpy(pattern, x_pattern, sizeof(pattern));
    }
    else if (c == 'Y') {
        bool y_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0},
            {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}
        };
        memcpy(pattern, y_pattern, sizeof(pattern));
    }
    else if (c == 'K') {
        bool k_pattern[7][5] = {
            {1,0,0,0,1}, {1,0,0,1,0}, {1,0,1,0,0}, {1,1,0,0,0},
            {1,0,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1}
        };
        memcpy(pattern, k_pattern, sizeof(pattern));
    }
    else if (c == '/') {
        bool slash_pattern[7][5] = {
            {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0},
            {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}
        };
        memcpy(pattern, slash_pattern, sizeof(pattern));
    }
    else if (c == ':') {
        bool colon_pattern[7][5] = {
            {0,0,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0},
            {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}
        };
        memcpy(pattern, colon_pattern, sizeof(pattern));
    }
    else if (c == '+') {
        bool plus_pattern[7][5] = {
            {0,0,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1},
            {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}
        };
        memcpy(pattern, plus_pattern, sizeof(pattern));
    }
    else if (c == '-') {
        bool minus_pattern[7][5] = {
            {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,1},
            {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}
        };
        memcpy(pattern, minus_pattern, sizeof(pattern));
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
