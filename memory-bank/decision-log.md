# Decision Log

## Decision 1
- **Date:** [Date]
- **Context:** [Context]
- **Decision:** [Decision]
- **Alternatives Considered:** [Alternatives]
- **Consequences:** [Consequences]

## Decision 2
- **Date:** [Date]
- **Context:** [Context]
- **Decision:** [Decision]
- **Alternatives Considered:** [Alternatives]
- **Consequences:** [Consequences]

## Technology Stack Selection
- **Date:** 2025-06-08 12:42:26 AM
- **Author:** Unknown User
- **Context:** Needed to choose appropriate technologies for building a Doom-style raycasting engine that would be educational, performant, and cross-platform compatible.
- **Decision:** Selected C++17 with SDL2 library and CMake build system
- **Alternatives Considered:** 
  - Python with Pygame (slower but simpler)
  - JavaScript with HTML5 Canvas (web-based but limited performance)
  - Rust with graphics crate (modern but steeper learning curve)
  - C with raw graphics libraries (maximum performance but complex development)
- **Consequences:** 
  - ✅ Excellent performance suitable for 60 FPS target
  - ✅ Cross-platform compatibility (macOS, Linux, Windows)
  - ✅ Direct hardware access through SDL2
  - ✅ Educational value showing low-level graphics programming
  - ✅ Industry-standard tools and practices
  - ⚠️ Requires manual memory management
  - ⚠️ More complex than higher-level languages
  - ⚠️ Requires SDL2 dependency installation

## Code Architecture Design
- **Date:** 2025-06-08 12:42:32 AM
- **Author:** Unknown User
- **Context:** Designing the codebase architecture to be maintainable, educational, and extensible for future development.
- **Decision:** Implemented modular architecture with separated classes: Game, Player, Map, Renderer, TextureManager, and HUD
- **Alternatives Considered:** 
  - Single monolithic source file
  - Header-only implementation
  - Static library approach
  - Dynamic plugin architecture
- **Consequences:** 
  - ✅ Clear separation of concerns
  - ✅ Easy to maintain and extend
  - ✅ Reusable components
  - ✅ Logical code organization
  - ✅ Follows modern C++ best practices
  - ⚠️ More files to manage
  - ⚠️ Requires proper header dependencies

## Texture System Implementation
- **Date:** 2025-06-08 12:42:39 AM
- **Author:** Unknown User
- **Context:** Needed to implement wall textures without requiring external image files while maintaining visual appeal and educational value.
- **Decision:** Implemented procedural texture generation with three distinct wall types (brick, stone, metal) using algorithmic patterns
- **Alternatives Considered:** 
  - Basic solid color walls
  - Simple bitmap texture loading
  - Procedural texture generation
  - Advanced shader-based texturing
- **Consequences:** 
  - ✅ No external texture file dependencies
  - ✅ Consistent visual style
  - ✅ Easy to modify and extend
  - ✅ Demonstrates algorithmic texture creation
  - ✅ Reduced project file size
  - ⚠️ Limited visual variety compared to bitmap textures
  - ⚠️ Requires more code to generate patterns

## HUD and Font System Design
- **Date:** 2025-06-08 12:42:52 AM
- **Author:** Unknown User
- **Context:** Needed to create a functional HUD system that displayed game information while maintaining the retro aesthetic and avoiding external font dependencies.
- **Decision:** Created custom bitmap font system with 7-segment display style numbers and pixel art characters
- **Alternatives Considered:** 
  - Minimal text-only HUD
  - External GUI library (Dear ImGui)
  - Bitmap font loading
  - Vector-based font rendering
- **Consequences:** 
  - ✅ Self-contained without external dependencies
  - ✅ Retro aesthetic matching game style
  - ✅ Educational value showing pixel-level rendering
  - ✅ Complete control over appearance
  - ✅ No additional library requirements
  - ⚠️ Limited character set
  - ⚠️ More code required for text rendering
  - ⚠️ Manual character pattern definition

## Lighting System Architecture Choice
- **Date:** 2025-06-08 12:52:35 AM
- **Author:** Unknown User
- **Context:** The game needed atmospheric lighting to enhance the exploration experience while maintaining the classic Doom-style raycasting performance and aesthetic.
- **Decision:** Implemented real-time per-ray directional lighting system with distance attenuation and cone-based flashlight illumination
- **Alternatives Considered:** 
  - Static pre-calculated lighting maps
  - Simple distance-based brightness only
  - Full real-time global illumination
  - Per-vertex lighting interpolation
  - Shader-based lighting (requires OpenGL)
- **Consequences:** 
  - ✅ Realistic flashlight behavior with cone-shaped beam
  - ✅ Maintains 60 FPS performance with optimized calculations
  - ✅ Adds gameplay depth and atmospheric exploration
  - ✅ Modular design allows easy parameter tweaking
  - ✅ Preserves retro aesthetic while adding modern lighting
  - ⚠️ Increases computational complexity per ray
  - ⚠️ May reduce performance on very low-end hardware
