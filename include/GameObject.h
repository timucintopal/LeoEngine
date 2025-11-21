#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Texture.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class GameObject {
public:
  GameObject(glm::vec2 position, glm::vec2 size, Texture* texture);
  ~GameObject();

  void Draw(const glm::mat4& view, const glm::mat4& projection, 
            GLuint shaderProgram, GLuint VAO, int indexCount = 6,
            bool useColor = false, glm::vec4 color = glm::vec4(1.0f));

  glm::vec2 position;
  glm::vec2 size;
  Texture* texture;
};

#endif // GAMEOBJECT_H

