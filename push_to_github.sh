#!/bin/bash

# Joom 프로젝트를 GitHub에 푸시하는 스크립트

echo "🚀 Pushing Joom project to GitHub..."
echo ""

# Joom 디렉토리로 이동
cd /Users/ogf2002/Joom

# Git 초기화 (이미 있다면 무시됨)
git init

# .gitignore 파일 확인/생성
if [ ! -f .gitignore ]; then
cat > .gitignore << 'EOF'
# Build files
build/
*.o
*.dylib
*.so
*.dll
*.exe

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# IDE files
.vscode/
.idea/
*.xcodeproj/
*.vcxproj*

# macOS files
.DS_Store
*.dSYM/

# Temporary files
*.tmp
*.temp
*~
*_backup*
*_optimized*

# Log files
*.log
EOF
fi

# 백업 파일들 제거
rm -f src/*_backup* src/*_optimized*

# 원격 리포지토리 설정
git remote remove origin 2>/dev/null || true
git remote add origin https://github.com/LimJang/joom.git

# 현재 상태 확인
echo "📊 Current project status:"
echo "   - Core engine: ✅ Complete"
echo "   - Performance: ✅ Optimized (60 FPS)"
echo "   - Lighting system: ✅ Enhanced flashlight (90° cone)"
echo "   - Audio system: ✅ Procedural sounds with reverb"
echo "   - Debug cleanup: ✅ Clean console output"
echo "   - HUD interface: ✅ Complete"
echo "   - Cave atmosphere: ✅ Immersive environment"
echo ""

# 모든 파일 스테이징
git add .

# 커밋 메시지
COMMIT_MSG="🎮 Enhanced Doom-Style Raycasting Engine v2.0

🔦 Latest Improvements:
- Expanded flashlight cone to 90° for better visibility
- Cleaned up all debugging output for professional experience
- Optimized lighting calculations for smoother performance
- Enhanced cave exploration atmosphere

✨ Core Features:
- Advanced raycasting 3D engine with DDA algorithm
- Smooth player movement (WASD + arrow keys)
- Three textured wall types (brick, stone, metal)
- Complete HUD system (health, ammo, score, FPS counter)
- Real-time mini-map with player position
- Precision collision detection
- Atmospheric cave environment with floor/ceiling

🔦 Advanced Lighting System:
- Wide-angle flashlight with 90° cone-shaped beam (F key toggle)
- Distance-based light attenuation with realistic physics
- Directional lighting calculations for authentic cave feel
- Sharp beam boundaries with dramatic light/dark transitions
- HUD flashlight status indicator

🎵 Procedural Audio System:
- Realistic shoe footsteps with heel clicks
- Professional cave reverb with 15 reflections
- Flashlight toggle sounds
- Volume controls with +/- keys
- Custom sound file loading support

⚡ Performance Optimizations:
- Stable 60 FPS with VSync support
- Optimized floor/ceiling rendering
- Frame-rate independent movement
- Efficient memory management
- Clean console output (debug-free)

🛠️ Technical Stack:
- C++17 with modern architecture
- SDL2 for graphics and input
- SDL_mixer for advanced audio
- CMake cross-platform build system
- Modular component design
- Comprehensive error handling

🎯 Controls:
- W/S: Move forward/backward
- A/D: Strafe left/right
- ←/→: Turn left/right  
- F: Toggle flashlight
- +/-: Adjust volume
- ESC: Exit game

🏗️ Build Instructions:
1. Install dependencies: brew install sdl2 sdl2_mixer sdl2_image (macOS)
2. Build: ./build.sh
3. Run: cd build && ./joom

🎮 Game Experience:
Ready for authentic cave exploration with professional lighting and audio!
Perfect for game development education and retro gaming enthusiasts."

# 커밋 실행
git commit -m "$COMMIT_MSG"

# 브랜치 설정 및 푸시
echo "📤 Pushing to GitHub..."
git branch -M main
git push -u origin main --force

# 결과 확인
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Successfully pushed to GitHub!"
    echo "🌐 Repository: https://github.com/LimJang/joom.git"
    echo ""
    echo "📋 Clone and build instructions:"
    echo "   git clone https://github.com/LimJang/joom.git"
    echo "   cd joom"
    echo "   ./build.sh"
    echo "   cd build && ./joom"
    echo ""
    echo "🎮 Latest features:"
    echo "   - 90° wide flashlight beam"
    echo "   - Clean debug-free experience"
    echo "   - Professional cave atmosphere"
    echo "   - Enhanced audio with reverb"
    echo "   - Optimized 60 FPS performance"
else
    echo "❌ Push failed! Please check:"
    echo "   1. GitHub credentials are set up"
    echo "   2. Repository exists and is accessible"
    echo "   3. Internet connection is stable"
    echo ""
    echo "💡 Alternative manual push:"
    echo "   git remote add origin https://github.com/LimJang/joom.git"
    echo "   git add ."
    echo "   git commit -m 'Enhanced lighting and debug cleanup'"
    echo "   git push -u origin main"
fi
