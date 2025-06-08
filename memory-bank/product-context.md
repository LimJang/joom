# Joom - Doom-Style Raycasting Engine

## Project Overview
Joom is a 3D raycasting game engine inspired by the classic id Software's Doom (1993). Built from scratch using C++ and SDL2, it implements the fundamental raycasting algorithm that made early 3D games possible on limited hardware.

## Core Objectives
- **Educational Purpose**: Demonstrate classic 3D rendering techniques
- **Performance**: Achieve 60 FPS on modern hardware
- **Modularity**: Clean, maintainable codebase with separated concerns
- **Cross-Platform**: Support for macOS, Linux, and Windows

## Target Audience
- Game development students learning 3D graphics fundamentals
- Retro gaming enthusiasts
- Developers interested in classic rendering techniques
- Anyone wanting to understand how early 3D games worked

## Key Features Implemented

### 🎮 Core Engine
- **Raycasting Algorithm**: DDA-based wall detection and distance calculation
- **Real-time Rendering**: 60 FPS target with optimized performance
- **Player Physics**: Smooth movement with collision detection
- **Map System**: Grid-based level representation

### 🎨 Graphics & Visuals
- **Textured Walls**: Three distinct wall types (brick, stone, metal)
- **Color-based Texturing**: Procedural texture generation
- **Mini-map**: Real-time overhead view with player position
- **Crosshair**: Center-screen aiming reticle

### 📊 User Interface
- **HUD System**: Complete heads-up display
- **Health Bar**: Visual health indicator with color gradients
- **Ammo Counter**: Current/maximum ammunition display
- **Score Display**: Real-time score tracking
- **FPS Counter**: Performance monitoring
- **Retro Aesthetics**: 7-segment display style numbers

### 🛠️ Technical Architecture
- **Modular Design**: Separated components for maintainability
- **Memory Management**: Proper resource allocation and cleanup
- **Build System**: CMake for cross-platform compilation
- **SDL2 Integration**: Graphics, input, and window management

## Technical Specifications

### Performance Targets
- **Frame Rate**: 60 FPS sustained
- **Resolution**: 800x600 (configurable)
- **Memory Usage**: < 10MB runtime
- **CPU Usage**: Single-threaded, optimized for efficiency

### System Requirements
- **OS**: macOS 10.14+, Ubuntu 18.04+, Windows 10+
- **Memory**: 512MB RAM minimum
- **Graphics**: Any GPU with basic 2D acceleration
- **Dependencies**: SDL2 library

## File Structure
```
joom/
├── src/                    # Source implementation files
│   ├── main.cpp           # Application entry point
│   ├── Game.cpp           # Game loop and state management
│   ├── Player.cpp         # Player movement and physics
│   ├── Map.cpp            # Level data and collision detection
│   ├── Renderer.cpp       # Raycasting and 3D rendering
│   ├── HUD.cpp            # User interface elements
│   └── TextureManager.cpp # Texture creation and management
├── include/               # Header declarations
├── build/                 # Compiled binaries (generated)
├── CMakeLists.txt         # Build configuration
├── README.md              # Project documentation
└── memory-bank/           # Development progress tracking
```

## Technology Stack
- **Language**: C++17
- **Graphics Library**: SDL2 (Simple DirectMedia Layer)
- **Build System**: CMake 3.16+
- **Platform Support**: Cross-platform (POSIX/Windows)
- **Development Tools**: Modern C++ compilers (GCC, Clang, MSVC)
