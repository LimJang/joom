#!/bin/bash

echo "🔧 Building Joom with Cave Environment..."
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
        echo "🆕 UPDATES:"
        echo "   ✅ Fixed F-key input issue (no more rapid toggling)"
        echo "   🏰 Added floor and ceiling rendering (cave environment)"
        echo "   💡 Enhanced atmospheric lighting system"
        echo ""
        echo "🌟 Features:"
        echo "   🔦 Realistic flashlight with cone beam"
        echo "   🏰 Underground cave atmosphere"
        echo "   🎯 Stone floor and ceiling with dynamic lighting"
        echo "   📊 Complete HUD system"
        echo "   🗺️ Enhanced mini-map"
        echo ""
        echo "🎮 Controls:"
        echo "   W/S - Move forward/backward"
        echo "   A/D - Strafe left/right" 
        echo "   ←/→ - Turn left/right"
        echo "   F - Toggle flashlight (FIXED - single press only!)"
        echo "   ESC - Exit game"
        echo ""
        echo "💡 Pro Tips:"
        echo "   • Turn off flashlight to experience the cave darkness"
        echo "   • Notice how floor and ceiling respond to lighting"
        echo "   • Explore different areas to see varied wall textures"
        echo ""
        echo "Welcome to the underground cave system! 🕳️✨"
    else
        echo "❌ Compilation failed!"
        exit 1
    fi
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
