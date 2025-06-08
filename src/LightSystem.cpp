#include "LightSystem.h"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LightSystem::LightSystem() 
    : flashlightIntensity(1.2f), flashlightRange(6.0f), 
      flashlightConeAngle(M_PI / 2.0f), ambientLight(0.05f), flashlightEnabled(true) {
}

LightSystem::~LightSystem() {
}

void LightSystem::toggleFlashlight() {
    flashlightEnabled = !flashlightEnabled;
}

void LightSystem::setFlashlightIntensity(float intensity) {
    flashlightIntensity = std::clamp(intensity, 0.0f, 2.0f);
}

void LightSystem::setFlashlightRange(float range) {
    flashlightRange = std::max(1.0f, range);
}

void LightSystem::setAmbientLight(float ambient) {
    ambientLight = std::clamp(ambient, 0.0f, 1.0f);
}

float LightSystem::calculateLighting(float playerX, float playerY, float playerAngle,
                                   float targetX, float targetY, float distance) const {
    // 매우 어두운 기본 환경 (99.9% 어둠)
    float totalLight = ambientLight * 0.001f;
    
    if (flashlightEnabled) {
        // 타겟 방향 계산
        float deltaX = targetX - playerX;
        float deltaY = targetY - playerY;
        float targetAngle = atan2(deltaY, deltaX);
        
        // 방향성 조명 계산
        float directionalLight = calculateDirectionalLight(playerAngle, targetAngle, distance);
        
        // 거리 감쇠 적용
        float attenuation = calculateDistanceAttenuation(distance);
        
        // 손전등 조명 추가 (극적인 반차)
        if (directionalLight > 0.0f) {
            totalLight += flashlightIntensity * directionalLight * attenuation;
        }
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
        // 중심에서 가장자리로 갈수록 극적으로 밝기 감소
        float normalizedAngle = angleDiff / flashlightConeAngle;
        
        // 더 극적인 감쇠 커브 (낙하산 형태)
        float coneIntensity = cos(normalizedAngle * M_PI / 2.0f);
        coneIntensity = pow(coneIntensity, 4.0f); // 4제곱으로 더욱 극적인 감쇠
        
        return coneIntensity;
    }
    
    return 0.0f; // 원뿔 범위 밖은 완전히 어둠
}

float LightSystem::calculateDistanceAttenuation(float distance) const {
    if (distance <= 0.0f) return 1.0f;
    
    // 올바른 거리 감쇠 공식 (최대값 1.0)
    float attenuation = 1.0f / (1.0f + 0.3f * distance + 0.15f * distance * distance);
    
    // 손전등 사거리 제한
    if (distance > flashlightRange) {
        float fadeDistance = 1.5f;
        float fadeStart = flashlightRange - fadeDistance;
        
        if (distance > fadeStart && fadeStart > 0) {
            float fadeRatio = (distance - fadeStart) / fadeDistance;
            
            // fadeRatio가 1 이상이면 완전 어둠
            if (fadeRatio >= 1.0f) {
                attenuation *= 0.01f;
            } else {
                attenuation *= std::max(0.01f, pow(1.0f - fadeRatio, 4.0f));
            }
        } else if (distance > flashlightRange) {
            attenuation *= 0.01f;
        }
    }
    
    // 감쇠값이 1을 초과하지 않도록 수동 제한 (0 미만도 방지)
    if (attenuation > 1.0f) {
        attenuation = 0.01f;
    } else if (attenuation < 0.0f) {
        attenuation = 0.0f;
    }
    
    return attenuation;
}
