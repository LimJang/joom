#!/bin/bash

# Joom 빌드 스크립트 (오디오 시스템 포함)

echo "🎮 Building Joom with Audio System..."

# SDL2_mixer 설치 확인
echo "🔍 Checking SDL2_mixer dependency..."

if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    if ! brew list sdl2_mixer &>/dev/null; then
        echo "⚠️  SDL2_mixer not found. Installing via Homebrew..."
        brew install sdl2_mixer
    else
        echo "✅ SDL2_mixer is already installed"
    fi
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    if ! pkg-config --exists SDL2_mixer; then
        echo "⚠️  SDL2_mixer not found. Please install it:"
        echo "   Ubuntu/Debian: sudo apt-get install libsdl2-mixer-dev"
        echo "   CentOS/RHEL: sudo yum install SDL2_mixer-devel"
        echo "   Arch: sudo pacman -S sdl2_mixer"
        read -p "Continue without audio? (y/n): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    else
        echo "✅ SDL2_mixer is available"
    fi
fi

# build 디렉토리의 기존 파일들 정리
cd build
rm -rf *

# CMake 설정
echo "🔧 Configuring with CMake..."
cmake ..

# 컴파일
echo "🔨 Compiling..."
make

# 성공 메시지
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful! Run './Joom' to start the game."
    echo ""
    echo "🎮 Controls:"
    echo "  W/S - Move forward/backward"
    echo "  A/D - Strafe left/right"
    echo "  Left/Right arrows - Turn"
    echo "  F - Toggle flashlight"
    echo "  +/- - Volume control"
    echo "  ESC - Exit"
    echo ""
    echo "🎵 Audio Features:"
    echo "  - Footstep sounds with cave echoes"
    echo "  - Flashlight toggle sound"
    echo "  - UI beep sounds"
    echo "  - Real-time volume control"
else
    echo ""
    echo "❌ Build failed!"
    echo ""
    echo "💡 Troubleshooting:"
    echo "  1. Make sure SDL2 and SDL2_mixer are installed"
    echo "  2. Check CMake version (3.16+ required)"
    echo "  3. Verify C++17 compiler support"
fi
