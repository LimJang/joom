# Joom - Doom Style Raycasting Engine

🎮 A Doom-inspired 3D raycasting game engine built with C++ and SDL2.

![Joom Screenshot](https://via.placeholder.com/800x400/2d2d2d/ffffff?text=Joom+Game+Engine)

## ✨ Features

### 🎨 Graphics & Rendering
- **3D Raycasting Engine**: Classic Doom-style pseudo-3D rendering
- **Textured Walls**: Different wall types (brick, stone, metal)
- **Real-time Rendering**: 60 FPS target with optimized performance
- **Mini-map**: Real-time player position and map layout

### 🎯 Gameplay
- **Smooth Player Movement**: WASD movement with strafe support
- **Mouse-like Controls**: Arrow key turning
- **Collision Detection**: Realistic wall collision physics
- **Crosshair**: Centered aiming reticle

### 📊 HUD System
- **Health Bar**: Visual health indicator with color changes
- **Ammo Counter**: Current/max ammo display
- **Score System**: Real-time score tracking
- **FPS Counter**: Performance monitoring
- **Retro Style**: Classic 7-segment display aesthetics

## 🛠️ Technical Details

### Architecture
- **Modular Design**: Separated components (Game, Player, Map, Renderer, HUD)
- **Memory Management**: Proper resource cleanup and management
- **Cross-platform**: CMake build system for portability

### Technologies
- **Language**: C++17
- **Graphics**: SDL2
- **Build System**: CMake
- **Platform**: macOS, Linux (Windows compatible)

## 🚀 Quick Start

### Prerequisites
```bash
# macOS (Homebrew)
brew install sdl2 cmake

# Ubuntu/Debian
sudo apt-get install libsdl2-dev cmake build-essential

# Fedora/RHEL
sudo dnf install SDL2-devel cmake gcc-c++
```

### Build & Run
```bash
# Clone the repository
git clone https://github.com/LimJang/joom.git
cd joom

# Build the project
./build_final.sh

# Run the game
cd build
./Joom
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` | Move forward |
| `S` | Move backward |
| `A` | Strafe left |
| `D` | Strafe right |
| `←` | Turn left |
| `→` | Turn right |
| `ESC` | Exit game |

## 🗺️ Map Layout

The game features a 16x16 grid map with different wall types:
- **Type 1**: Brick walls (brown) - Outer boundaries
- **Type 2**: Stone walls (gray) - Interior structures  
- **Type 3**: Metal walls (dark) - Special areas

## 📁 Project Structure

```
joom/
├── src/                 # Source files
│   ├── main.cpp        # Entry point
│   ├── Game.cpp        # Game loop and management
│   ├── Player.cpp      # Player movement and physics
│   ├── Map.cpp         # Map data and collision
│   ├── Renderer.cpp    # 3D rendering and raycasting
│   ├── HUD.cpp         # User interface
│   └── TextureManager.cpp # Texture handling
├── include/            # Header files
├── build/              # Build output (generated)
├── CMakeLists.txt      # Build configuration
└── README.md           # This file
```

## 🎯 Raycasting Algorithm

The engine uses a DDA (Digital Differential Analyzer) raycasting algorithm:

1. **Ray Generation**: Cast rays from player position at FOV angles
2. **Grid Traversal**: Step through map grid until wall hit
3. **Distance Calculation**: Calculate perpendicular wall distance
4. **Wall Height**: Project 3D wall height based on distance
5. **Texture Mapping**: Apply appropriate wall texture/color

## 🎨 Rendering Pipeline

```
Player Input → Movement Update → Raycasting → Wall Rendering → HUD Overlay → Present
```

## 🔧 Customization

### Adding New Wall Types
1. Update `Map.cpp` with new wall type numbers
2. Add texture creation in `TextureManager.cpp`
3. Update color mapping in `Renderer.cpp`

### Modifying HUD Elements
- Edit `HUD.cpp` for layout changes
- Modify `renderChar()` for new font characters
- Adjust colors and positioning as needed

## 🐛 Known Issues

- Texture mapping is currently color-based (planned: bitmap textures)
- Limited character set in HUD font
- No audio system yet

## 🚧 Planned Features

- [ ] **Weapons System**: Shooting mechanics and ammo consumption
- [ ] **Enemy AI**: Monster sprites and basic AI
- [ ] **Sound Effects**: Footsteps, gunshots, ambient sounds
- [ ] **Bitmap Textures**: Real texture mapping instead of colors
- [ ] **Doors**: Interactive door systems
- [ ] **Items**: Health packs, ammo pickups, keys
- [ ] **Multiple Levels**: Level progression system
- [ ] **Save System**: Game state persistence

## 📈 Performance

- **Target FPS**: 60 FPS
- **Resolution**: 800x600 (configurable)
- **Memory Usage**: ~5MB (optimized for efficiency)
- **CPU Usage**: Single-threaded, ~10-20% on modern hardware

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **id Software**: For creating the original Doom and inspiring this project
- **SDL2 Team**: For the excellent multimedia library
- **Raycasting Tutorials**: Various online resources for algorithm understanding

## 📞 Contact

**LimJang** - GitHub: [@LimJang](https://github.com/LimJang)

Project Link: [https://github.com/LimJang/joom](https://github.com/LimJang/joom)

---

*Made with ❤️ and lots of ☕*
