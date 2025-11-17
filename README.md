# Joom - Infinite Cave Explorer

🎮 A high-performance 3D raycasting engine demo built with C++17 and SDL2, designed for exploring atmospheric, procedurally-generated cave systems.

![Joom Screenshot](https://via.placeholder.com/800x400/2d2d2d/ffffff?text=Joom+Engine+Demo)

## ✨ Features

### 🎨 Graphics & Rendering
- **Optimized Raycasting Engine**: A high-performance hybrid renderer achieves 60 FPS on a wide range of hardware. It uses a fast horizontal scanline algorithm for floors and ceilings, and vertical raycasting for walls.
- **Fully Textured Environment**: All surfaces (walls, floors, ceilings) are textured using a highly optimized texture manager that caches raw pixel data for rapid CPU-side sampling.
- **Dynamic Lighting**: A simple but effective lighting system featuring a player-controlled flashlight and ambient light, with a pre-calculated distance-based falloff look-up table.
- **Buffered Rendering**: The entire scene is drawn to an off-screen pixel buffer (`SDL_Texture` with streaming) before being presented to the screen, preventing visual tearing and improving performance.

### 🔊 Audio
- **Atmospheric Sound**: Background ambient sounds create an immersive cave-like atmosphere.
- **Dynamic Footsteps**: The player's footsteps alternate between two different `.wav` files, creating a realistic walking sound effect.
- **Volume Control**: In-game master volume control.

### 📊 HUD System
- **Real-time Info**: Displays FPS, current level, and other critical information.
- **Status Indicators**: Visual indicators for flashlight status and audio volume.
- **Retro Aesthetics**: Uses a custom-rendered 7-segment display for numbers, adding to the classic feel.

## 🛠️ Technical Details

### Core Architecture
The project follows a centralized architecture where the `Game` class owns and manages all major subsystems.

| Class | Responsibility |
| --- | --- |
| **`Game`** | The central hub that runs the game loop and coordinates all other systems. |
| **`Renderer`** | Renders the 3D world using a hybrid raycasting approach. |
| **`Player`** | Manages the player's position, orientation, and movement. |
| **`Map`** | Loads and manages the tile-based map data for each level. |
| **`AudioManager`**| Manages and plays all sound effects and background music. |
| **`HUD`** | Renders the 2D Heads-Up Display. |
| **`ItemManager`**| Manages the state of in-game items (though most have been removed). |
| **`LightSystem`**| Manages the flashlight and ambient lighting. |
| **`TextureManager`**| Loads and provides optimized access to all textures. |

### Technologies
- **Language**: C++17
- **Libraries**: SDL2, SDL2_image, SDL2_mixer
- **Build System**: CMake

## 🚀 Quick Start

### Prerequisites
```bash
# macOS (Homebrew)
brew install sdl2 sdl2_image sdl2_mixer cmake

# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev cmake build-essential
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
./Joom.app/Contents/MacOS/Joom
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` | Move forward |
| `S` | Move backward |
| `A` | Strafe left |
| `D` | Strafe right |
| `F` | Toggle Flashlight |
| `+` | Increase Volume |
| `-` | Decrease Volume |
| `ESC` | Exit game |

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **id Software**: For creating the original Doom and inspiring this project.
- **Lode Vandevenne**: For the excellent raycasting tutorial.
- **SDL2 Team**: For the fantastic multimedia library.