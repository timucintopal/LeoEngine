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
add_executable(LeoEngine src/main.cpp src/Game.cpp src/Texture.cpp src/GameObject.cpp src/Camera.cpp)

# Include directories
target_include_directories(LeoEngine PRIVATE ${stb_SOURCE_DIR})

# Link Libraries
target_link_libraries(LeoEngine PRIVATE SDL2::SDL2 GLEW::GLEW OpenGL::GL glm::glm)
