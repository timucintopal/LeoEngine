cmake_minimum_required(VERSION 3.24)
project(LeoEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(include)

# FetchContent for SDL2
include(FetchContent)
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.28.5
)
FetchContent_MakeAvailable(SDL2)

# Try to find SDL2_mixer system package first
find_package(SDL2_mixer QUIET)
if(NOT SDL2_mixer_FOUND)
  message(STATUS "SDL2_mixer not found, attempting to build from source...")
  # FetchContent for SDL2_mixer
  FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-2.8.0
  )
  # Disable optional dependencies to avoid build issues
  set(SDL2_MIXER_OPUS OFF CACHE BOOL "" FORCE)
  set(SDL2_MIXER_OPUSFILE OFF CACHE BOOL "" FORCE)
  set(SDL2_MIXER_FLAC OFF CACHE BOOL "" FORCE)
  set(SDL2_MIXER_MOD OFF CACHE BOOL "" FORCE)
  set(SDL2_MIXER_MIDI OFF CACHE BOOL "" FORCE)
  set(SDL2_MIXER_MP3_MPG123 OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(SDL2_mixer)
endif()

# Try to find SDL2_ttf system package first
find_package(SDL2_ttf QUIET)
if(NOT SDL2_ttf_FOUND)
  message(STATUS "SDL2_ttf not found, attempting to build from source...")
  # FetchContent for SDL2_ttf
  FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.22.0
  )
  FetchContent_MakeAvailable(SDL2_ttf)
endif()

# Find GLEW
find_package(GLEW REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# FetchContent for GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# FetchContent for stb
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Add executable
add_executable(LeoEngine src/main.cpp src/Game.cpp src/Texture.cpp src/GameObject.cpp src/Camera.cpp src/CollisionManager.cpp src/TextRenderer.cpp src/Renderer.cpp src/InputManager.cpp src/ResourceManager.cpp src/Scene.cpp src/Animation.cpp)

# Include directories
target_include_directories(LeoEngine PRIVATE ${stb_SOURCE_DIR})

# Link Libraries
target_link_libraries(LeoEngine PRIVATE SDL2::SDL2 SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf GLEW::GLEW OpenGL::GL glm::glm)
