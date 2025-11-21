#include "TextRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

TextRenderer::TextRenderer() 
    : m_Font(nullptr), m_TextTexture(0), m_TextWidth(0), m_TextHeight(0) {}

TextRenderer::~TextRenderer() {
  Cleanup();
}

bool TextRenderer::LoadFont(const char* fontPath, int fontSize) {
  m_Font = TTF_OpenFont(fontPath, fontSize);
  if (m_Font == nullptr) {
    std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    return false;
  }
  std::cout << "Font loaded: " << fontPath << std::endl;
  return true;
}

void TextRenderer::CreateTextTexture(const std::string& text) {
  if (m_Font == nullptr) {
    return;
  }

  // Render text to SDL surface with black text on white background
  SDL_Color textColor = {0, 0, 0, 255}; // Black
  SDL_Color bgColor = {255, 255, 255, 255}; // White
  SDL_Surface* textSurface = TTF_RenderText_Shaded(m_Font, text.c_str(), textColor, bgColor);
  
  if (textSurface == nullptr) {
    std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    return;
  }

  m_TextWidth = textSurface->w;
  m_TextHeight = textSurface->h;

  // Delete old texture if exists
  if (m_TextTexture != 0) {
    glDeleteTextures(1, &m_TextTexture);
  }

  // Create OpenGL texture
  glGenTextures(1, &m_TextTexture);
  glBindTexture(GL_TEXTURE_2D, m_TextTexture);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Convert SDL surface to OpenGL texture
  // TTF_RenderText_Solid creates a surface with format SDL_PIXELFORMAT_RGBA32 or similar
  // We need to determine the correct format
  GLenum textureFormat = GL_RGBA;
  GLenum dataType = GL_UNSIGNED_BYTE;
  
  // Check surface format and convert if necessary
  if (textSurface->format->format == SDL_PIXELFORMAT_RGBA32 || 
      textSurface->format->format == SDL_PIXELFORMAT_ABGR8888) {
    textureFormat = GL_RGBA;
  } else if (textSurface->format->format == SDL_PIXELFORMAT_BGRA32 ||
             textSurface->format->format == SDL_PIXELFORMAT_ARGB8888) {
    textureFormat = GL_BGRA;
  } else {
    // Default: assume RGBA and let SDL convert if needed
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(textSurface, SDL_PIXELFORMAT_RGBA32, 0);
    if (convertedSurface) {
      SDL_FreeSurface(textSurface);
      textSurface = convertedSurface;
      textureFormat = GL_RGBA;
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0,
               textureFormat, dataType, textSurface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Free SDL surface
  SDL_FreeSurface(textSurface);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(const std::string& text, int x, int y,
                              GLuint shaderProgram, GLuint VAO,
                              int screenWidth, int screenHeight) {
  if (m_Font == nullptr) {
    return;
  }

  // Create texture for this text
  CreateTextTexture(text);

  if (m_TextTexture == 0) {
    return;
  }

  // Use shader program
  glUseProgram(shaderProgram);

  // Screen space projection (no view matrix, fixed orthographic)
  glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
  glm::mat4 view = glm::mat4(1.0f); // Identity matrix (no camera movement)
  
  // Model matrix - position text at screen coordinates
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3((float)x, (float)y, 0.0f));
  model = glm::scale(model, glm::vec3((float)m_TextWidth, (float)m_TextHeight, 1.0f));

  // Set uniforms
  unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Set texture mode (not color mode)
  unsigned int useColorLoc = glGetUniformLocation(shaderProgram, "useColor");
  glUniform1i(useColorLoc, 0);

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, m_TextTexture);

  // Draw
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void TextRenderer::Cleanup() {
  if (m_TextTexture != 0) {
    glDeleteTextures(1, &m_TextTexture);
    m_TextTexture = 0;
  }
  if (m_Font != nullptr) {
    TTF_CloseFont(m_Font);
    m_Font = nullptr;
  }
}

