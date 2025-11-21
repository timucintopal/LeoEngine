#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Texture.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Animation;

class GameObject {
public:
  GameObject(glm::vec2 position, glm::vec2 size, Texture* texture);
  ~GameObject();

  void Update(float deltaTime);
  void Draw(const glm::mat4& view, const glm::mat4& projection, 
            GLuint shaderProgram, GLuint VAO, int indexCount = 6,
            bool useColor = false, glm::vec4 color = glm::vec4(1.0f));

  glm::vec4 GetBoundingBox() const;

  glm::vec2 position;
  glm::vec2 size;
  Texture* texture;
  Animation* currentAnimation;
};

#endif // GAMEOBJECT_H

