#pragma once
#include <SDL2/SDL.h>
#include <string>

class HUD {
private:
    SDL_Renderer* renderer;
    int screenWidth, screenHeight;
    
    // HUD 상태
    int health;
    int maxHealth;
    int ammo;
    int maxAmmo;
    int score;
    float fps;
    
    // 조명 상태
    bool flashlightOn;
    
public:
    HUD(SDL_Renderer* sdlRenderer, int width, int height);
    ~HUD();
    
    // HUD 렌더링
    void render();
    
    // 상태 업데이트
    void setHealth(int h) { health = h; }
    void setAmmo(int a) { ammo = a; }
    void setScore(int s) { score = s; }
    void setFPS(float f) { fps = f; }
    void setFlashlightStatus(bool on) { flashlightOn = on; }
    
    // 상태 가져오기
    int getHealth() const { return health; }
    int getAmmo() const { return ammo; }
    int getScore() const { return score; }
    
private:
    // HUD 요소 렌더링 함수들
    void renderHealthBar();
    void renderAmmoCounter();
    void renderScore();
    void renderFPS();
    void renderCrosshair();
    void renderFlashlightStatus();
    
    // 텍스트/숫자 렌더링 (비트맵 폰트 스타일)
    void renderNumber(int number, int x, int y, int scale = 2);
    void renderText(const std::string& text, int x, int y, int scale = 1);
    void renderDigit(int digit, int x, int y, int scale);
    void renderChar(char c, int x, int y, int scale);
    
    // 기본 도형 그리기
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
};
