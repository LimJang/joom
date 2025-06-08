#!/bin/bash

echo "🔧 Building Joom with textures and HUD..."
echo ""

cd /Users/ogf2002/Joom/build
rm -f *
cmake ..

if cmake ..; then
    echo "✅ CMake configuration successful"
    echo ""
    
    if make; then
        echo ""
        echo "🎉 Build successful!"
        echo ""
        echo "🚀 Run the game with: ./Joom"
        echo ""
        echo "🆕 New Features Added:"
        echo "   🎨 Textured walls (brick=brown, stone=gray, metal=dark)"
        echo "   📊 HUD system (health bar, ammo counter, score, FPS)"
        echo "   🎯 Crosshair in center"
        echo "   🗺️ Enhanced map with different wall types"
        echo ""
        echo "🎮 Controls:"
        echo "   W/S - Move forward/backward"
        echo "   A/D - Strafe left/right" 
        echo "   ←/→ - Turn left/right"
        echo "   ESC - Exit game"
        echo ""
        echo "Ready to play! 🎮"
    else
        echo "❌ Compilation failed!"
        exit 1
    fi
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
