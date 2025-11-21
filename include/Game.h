#ifndef GAME_H
#define GAME_H

#include "Texture.h"
#include "GameObject.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "TextRenderer.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

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

  GLuint shaderProgram;
  GLuint VAO, VBO, EBO;
  GLuint circleVAO, circleVBO, circleEBO;
  int circleIndexCount;

  Texture *playerTexture;
  std::vector<GameObject*> m_GameObjects;
  
  Camera* m_Camera;
  int m_ScreenWidth;
  int m_ScreenHeight;
  
  Mix_Music* m_BackgroundMusic;
  Mix_Chunk* m_JumpSound;
  Mix_Chunk* m_CollisionSound;
  bool m_WasColliding;
  
  TextRenderer* m_TextRenderer;
};

#endif // GAME_H
