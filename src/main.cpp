#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

int main(int argc, char *argv[]) {
  // 1. Initialize SDL2 Video Subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  // 2. Configure OpenGL Attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __APPLE__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

  // 3. Create Window
  SDL_Window *window = SDL_CreateWindow("Wayne Engine", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, 800, 600,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

  // 4. Create OpenGL Context
  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    std::cerr << "OpenGL context could not be created! SDL_Error: "
              << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_GL_MakeCurrent(window, glContext);

  // 5. Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    std::cerr << "Error initializing GLEW: " << glewGetErrorString(glewError)
              << std::endl;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  std::cout << "Engine Initialized" << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

  // 6. Game Loop
  bool isRunning = true;
  SDL_Event event;

  while (isRunning) {
    // Handle Events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        isRunning = false;
      }
    }

    // Render
    // Clear screen with dark gray
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap buffers
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
