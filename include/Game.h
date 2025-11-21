#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Camera.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <iostream>

class Game {
public:
  Game();
  ~Game();

  void Init(const char *title, int width, int height, bool fullscreen);
  void HandleEvents();
  void Update(float deltaTime);
  void Render();
  void Clean();

  bool Running() { return isRunning; }

private:
  bool isRunning;
  SDL_Window *window;
  SDL_GLContext glContext;

  Renderer* m_Renderer;
  InputManager* m_InputManager;
  ResourceManager* m_ResourceManager;
  Scene* m_Scene;
  Camera* m_Camera;
  
  int m_ScreenWidth;
  int m_ScreenHeight;
  bool m_WasColliding;
  
  void InitSDL();
  void InitOpenGL();
  void InitResources();
  void InitScene();
};

#endif // GAME_H
