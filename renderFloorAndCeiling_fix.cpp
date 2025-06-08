void Renderer::renderFloorAndCeiling(Player* player, Map* map) {
    float playerX = player->getX();
    float playerY = player->getY();
    float playerAngle = player->getAngle();
    
    // 화면의 각 열에 대해 바닥/천장 조명 계산
    float startAngle = playerAngle - degreesToRadians(FOV / 2);
    float angleIncrement = degreesToRadians(FOV) / screenWidth;
    
    // 성능 최적화: 4픽셀마다 계산하고 나머지는 보간
    for (int x = 0; x < screenWidth; x += 4) {
        float rayAngle = startAngle + x * angleIncrement;
        float cosAngle = cos(rayAngle - playerAngle);
        
        // 바닥 렌더링 (화면 중앙 아래)
        for (int y = screenHeight / 2 + 1; y < screenHeight; y += 8) { // 8픽셀마다
            float distance = (1.0f * screenHeight) / (2.0f * (y - screenHeight / 2.0f)) / cosAngle;
            
            if (distance > 12.0f) continue; // 너무 먼 곳은 생략 (손전등 범위 고려)
            
            float worldX = playerX + cos(rayAngle) * distance;
            float worldY = playerY + sin(rayAngle) * distance;
            
            // 실제 손전등 조명 계산
            float floorLighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, worldX, worldY, distance);
            
            // 바닥 색상 (돌 질감)
            Uint8 floorR = static_cast<Uint8>(80 * floorLighting);
            Uint8 floorG = static_cast<Uint8>(70 * floorLighting);
            Uint8 floorB = static_cast<Uint8>(60 * floorLighting);
            
            SDL_SetRenderDrawColor(renderer, floorR, floorG, floorB, 255);
            
            // 4x8 픽셀 블록 렌더링
            for (int dx = 0; dx < 4 && x + dx < screenWidth; dx++) {
                for (int dy = 0; dy < 8 && y + dy < screenHeight; dy++) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
        
        // 천장 렌더링 (화면 중앙 위)
        for (int y = 0; y < screenHeight / 2; y += 8) {
            float distance = (1.0f * screenHeight) / (2.0f * (screenHeight / 2.0f - y)) / cosAngle;
            
            if (distance > 12.0f) continue;
            
            float worldX = playerX + cos(rayAngle) * distance;
            float worldY = playerY + sin(rayAngle) * distance;
            
            // 실제 손전등 조명 계산
            float ceilingLighting = lightSystem->calculateLighting(playerX, playerY, playerAngle, worldX, worldY, distance);
            
            // 천장 색상 (어두운 돌)
            Uint8 ceilingR = static_cast<Uint8>(50 * ceilingLighting);
            Uint8 ceilingG = static_cast<Uint8>(45 * ceilingLighting);
            Uint8 ceilingB = static_cast<Uint8>(40 * ceilingLighting);
            
            SDL_SetRenderDrawColor(renderer, ceilingR, ceilingG, ceilingB, 255);
            
            // 4x8 픽셀 블록 렌더링
            for (int dx = 0; dx < 4 && x + dx < screenWidth; dx++) {
                for (int dy = 0; dy < 8 && y + dy >= 0; dy++) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }
}
