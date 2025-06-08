#include "LightSystem.h"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LightSystem::LightSystem() 
    : flashlightIntensity(0.8f), flashlightRange(10.0f), 
      flashlightConeAngle(M_PI / 6.0f), ambientLight(0.1f), flashlightEnabled(true) {
}

LightSystem::~LightSystem() {
}

void LightSystem::toggleFlashlight() {
    flashlightEnabled = !flashlightEnabled;
}

void LightSystem::setFlashlightIntensity(float intensity) {
    flashlightIntensity = std::clamp(intensity, 0.0f, 1.0f);
}

void LightSystem::setFlashlightRange(float range) {
    flashlightRange = std::max(1.0f, range);
}

void LightSystem::setAmbientLight(float ambient) {
    ambientLight = std::clamp(ambient, 0.0f, 1.0f);
}

float LightSystem::calculateLighting(float playerX, float playerY, float playerAngle,
                                   float targetX, float targetY, float distance) const {
    // 기본 환경광
    float totalLight = ambientLight;
    
    if (flashlightEnabled) {
        // 타겟 방향 계산
        float deltaX = targetX - playerX;
        float deltaY = targetY - playerY;
        float targetAngle = atan2(deltaY, deltaX);
        
        // 방향성 조명 계산
        float directionalLight = calculateDirectionalLight(playerAngle, targetAngle, distance);
        
        // 거리 감쇠 적용
        float attenuation = calculateDistanceAttenuation(distance);
        
        // 손전등 조명 추가
        totalLight += flashlightIntensity * directionalLight * attenuation;
    }
    
    // 최종 조명값을 0.0 ~ 1.0 범위로 제한
    return std::clamp(totalLight, 0.0f, 1.0f);
}

float LightSystem::calculateDirectionalLight(float playerAngle, float targetAngle, float distance) const {
    // 플레이어 시선과 타겟 방향의 각도 차이 계산
    float angleDiff = targetAngle - playerAngle;
    
    // 각도를 -π ~ π 범위로 정규화
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
    
    // 절대값 사용
    angleDiff = std::abs(angleDiff);
    
    // 손전등 원뿔 범위 내에 있는지 확인
    if (angleDiff <= flashlightConeAngle) {
        // 중심에서 가장자리로 갈수록 밝기 감소 (코사인 커브)
        float coneIntensity = cos(angleDiff / flashlightConeAngle * M_PI / 2.0f);
        return coneIntensity;
    }
    
    return 0.0f; // 원뿔 범위 밖
}

float LightSystem::calculateDistanceAttenuation(float distance) const {
    if (distance <= 0.0f) return 1.0f;
    
    // 거리에 따른 제곱 감쇠 (현실적인 조명)
    float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);
    
    // 손전등 사거리 제한
    if (distance > flashlightRange) {
        float fadeDistance = 2.0f; // 페이드 아웃 거리
        float fadeStart = flashlightRange - fadeDistance;
        
        if (distance > fadeStart) {
            float fadeRatio = (distance - fadeStart) / fadeDistance;
            attenuation *= std::max(0.0f, 1.0f - fadeRatio);
        }
    }
    
    return attenuation;
}
