#pragma once
#include <cmath>

class LightSystem {
private:
    float flashlightIntensity;      // 손전등 밝기 (0.0 ~ 1.0)
    float flashlightRange;          // 손전등 사거리
    float flashlightConeAngle;      // 손전등 원뿔각도 (라디안)
    float ambientLight;             // 환경 조명 (최소 밝기)
    bool flashlightEnabled;         // 손전등 켜짐/꺼짐
    
public:
    LightSystem();
    ~LightSystem();
    
    // 광원 제어
    void toggleFlashlight();
    void setFlashlightIntensity(float intensity);
    void setFlashlightRange(float range);
    void setAmbientLight(float ambient);
    
    // 조명 계산
    float calculateLighting(float playerX, float playerY, float playerAngle,
                           float targetX, float targetY, float distance) const;
    
    // 방향성 조명 계산 (손전등 원뿔)
    float calculateDirectionalLight(float playerAngle, float targetAngle, float distance) const;
    
    // 거리 기반 감쇠 계산
    float calculateDistanceAttenuation(float distance) const;
    
    // Getter 함수들
    bool isFlashlightEnabled() const { return flashlightEnabled; }
    float getFlashlightIntensity() const { return flashlightIntensity; }
    float getAmbientLight() const { return ambientLight; }
};
