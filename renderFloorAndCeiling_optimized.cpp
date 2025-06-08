void Renderer::renderFloorAndCeiling(Player* player, Map* map) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    
    // 손전등이 꺼져있으면 어두운 바닥/천장만 렌더링
    if (!lightSystem->isFlashlightEnabled()) {
        // 매우 어두운 색상
        SDL_SetRenderDrawColor(renderer, 8, 7, 6, 255); // 바닥
        SDL_Rect floorRect = {0, screenHeight / 2, screenWidth, screenHeight / 2};
        SDL_RenderFillRect(renderer, &floorRect);
        
        SDL_SetRenderDrawColor(renderer, 5, 4, 4, 255); // 천장
        SDL_Rect ceilingRect = {0, 0, screenWidth, screenHeight / 2};
        SDL_RenderFillRect(renderer, &ceilingRect);
        return;
    }
    
    // 손전등이 켜져있을 때만 원뿔 투영 계산
    
    // 화면 중심점 (손전등 방향)
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    
    // 손전등 원뿔의 화면상 반지름 계산 (거리에 따라 변함)
    float maxRadius = screenWidth * 0.4f; // 화면 너비의 40%
    
    // 성능 최적화: 큰 블록 단위로 렌더링
    for (int x = 0; x < screenWidth; x += 12) {
        for (int y = 0; y < screenHeight; y += 12) {
            // 화면 중심에서의 거리 계산
            float dx = x - centerX;
            float dy = y - centerY;
            float screenDistance = sqrt(dx * dx + dy * dy);
            
            // 바닥과 천장 구분
            bool isFloor = (y > screenHeight / 2);
            
            // 손전등 원뿔 범위 체크
            float coneRadius = maxRadius * (1.0f + abs(dy) / (screenHeight * 0.5f) * 0.5f);
            
            float lighting = 0.02f; // 기본 어둠
            
            if (screenDistance < coneRadius) {
                // 원뿔 내부 - 중심에서 가장자리로 갈수록 어두워짐
                float coneIntensity = 1.0f - (screenDistance / coneRadius);
                coneIntensity = pow(coneIntensity, 1.5f); // 자연스러운 감쇠
                
                // 바닥은 더 밝게, 천장은 덜 밝게
                if (isFloor) {
                    lighting = 0.02f + coneIntensity * 0.6f; // 바닥: 최대 62%
                } else {
                    lighting = 0.02f + coneIntensity * 0.3f; // 천장: 최대 32%
                }
            }
            
            // 색상 계산
            Uint8 r, g, b;
            if (isFloor) {
                r = static_cast<Uint8>(80 * lighting);
                g = static_cast<Uint8>(70 * lighting);
                b = static_cast<Uint8>(60 * lighting);
            } else {
                r = static_cast<Uint8>(50 * lighting);
                g = static_cast<Uint8>(45 * lighting);
                b = static_cast<Uint8>(40 * lighting);
            }
            
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            
            // 12x12 블록 렌더링
            for (int dx = 0; dx < 12 && x + dx < screenWidth; dx++) {
                for (int dy = 0; dy < 12 && y + dy < screenHeight; dy++) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }
}
