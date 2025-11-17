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
    float fps;
    
    // 시스템 상태
    bool flashlightOn;
    bool audioEnabled;
    int masterVolume;
    
    // 게임 상태
    bool hasRedKey;
    bool hasBlueKey;
    bool hasYellowKey;
    
public:
    HUD(SDL_Renderer* sdlRenderer, int width, int height);
    ~HUD();
    
    // HUD 렌더링
    void render();
    void renderLevelCompleteMessage(int level);
    
    // 상태 업데이트
    void setHealth(int h) { health = h; }
    void setAmmo(int a) { ammo = a; }
    void setFPS(float f) { fps = f; }
    void setFlashlightStatus(bool on) { flashlightOn = on; }
    void setAudioStatus(bool enabled, int volume) { audioEnabled = enabled; masterVolume = volume; }
    void setKeyStatus(bool red, bool blue, bool yellow) { hasRedKey = red; hasBlueKey = blue; hasYellowKey = yellow; }
    
    // 상태 가져오기
    int getHealth() const { return health; }
    int getAmmo() const { return ammo; }
    
private:
    // HUD 요소 렌더링 함수들
    void renderHealthBar();
    void renderAmmoCounter();
    void renderFPS();
    void renderCrosshair();
    void renderFlashlightStatus();
    void renderAudioStatus();
    void renderControls();
    void renderKeyStatus();
    
    // 텍스트/숫자 렌더링 (비트맵 폰트 스타일)
    void renderNumber(int number, int x, int y, int scale = 2);
    void renderText(const std::string& text, int x, int y, int scale = 1);
    void renderDigit(int digit, int x, int y, int scale);
    void renderChar(char c, int x, int y, int scale);
    
    // 기본 도형 그리기
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    // 키 아이콘 그리기
    void drawKeyIcon(int x, int y, bool hasKey, Uint8 r, Uint8 g, Uint8 b);
};
