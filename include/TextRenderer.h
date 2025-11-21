#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class TextRenderer {
public:
  TextRenderer();
  ~TextRenderer();

  bool LoadFont(const char* fontPath, int fontSize);
  void RenderText(const std::string& text, int x, int y, 
                  GLuint shaderProgram, GLuint VAO, 
                  int screenWidth, int screenHeight);
  void Cleanup();

private:
  TTF_Font* m_Font;
  GLuint m_TextTexture;
  int m_TextWidth;
  int m_TextHeight;
  
  void CreateTextTexture(const std::string& text);
};

#endif // TEXTRENDERER_H

