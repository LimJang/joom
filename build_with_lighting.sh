#!/bin/bash

echo "🔧 Building Joom with Lighting System..."
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
        echo "💡 NEW: Flashlight Lighting System!"
        echo ""
        echo "🆕 Features:"
        echo "   💡 Player flashlight with realistic lighting"
        echo "   🔦 Directional cone lighting (30° spread)"
        echo "   🌑 Distance-based light attenuation"
        echo "   🎨 Textured walls (brick, stone, metal)"
        echo "   📊 HUD system (health, ammo, score, FPS, light status)"
        echo "   🎯 Crosshair and mini-map"
        echo ""
        echo "🎮 Controls:"
        echo "   W/S - Move forward/backward"
        echo "   A/D - Strafe left/right" 
        echo "   ←/→ - Turn left/right"
        echo "   F - Toggle flashlight ON/OFF"
        echo "   ESC - Exit game"
        echo ""
        echo "💡 Lighting Features:"
        echo "   • Realistic cone-shaped flashlight beam"
        echo "   • Dynamic shadows and brightness"
        echo "   • Ambient lighting for basic visibility"
        echo "   • Press F to experience the difference!"
        echo ""
        echo "Ready to explore in the dark! 🕯️"
    else
        echo "❌ Compilation failed!"
        exit 1
    fi
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
