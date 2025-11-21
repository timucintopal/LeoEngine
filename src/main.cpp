#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "Engine Initialized" << std::endl;

    SDL_Quit();
    return 0;
}
