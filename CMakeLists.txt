cmake_minimum_required(VERSION 3.16)
project(Joom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS Homebrew 경로 설정
if(APPLE)
    set(SDL2_ROOT "/opt/homebrew")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    
    # SDL2 헤더 경로
    include_directories("${SDL2_ROOT}/include/SDL2")
    include_directories("${SDL2_ROOT}/include")
    
    # SDL2 라이브러리 경로
    link_directories("${SDL2_ROOT}/lib")
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Map.cpp
    src/Renderer.cpp
    src/TextureManager.cpp
    src/HUD.cpp
    src/LightSystem.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        SDL2 
        SDL2main
        "-framework Cocoa" 
        "-framework IOKit" 
        "-framework CoreVideo"
        "-framework ForceFeedback"
        "-framework Carbon"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework Metal"
        "-framework QuartzCore"
    )
else()
    # Linux/Windows
    target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)
endif()

# Copy to build directory
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
