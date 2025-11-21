#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <SDL.h>
#include <iostream>

class Game {
public:
  Game();
  ~Game();

  void Init(const char *title, int width, int height, bool fullscreen);
  void HandleEvents();
  void Update();
  void Render();
  void Clean();

  bool Running() { return isRunning; }

private:
  bool isRunning;
  SDL_Window *window;
  SDL_GLContext glContext;

  GLuint shaderProgram;
  GLuint VAO, VBO;
};

#endif // GAME_H
