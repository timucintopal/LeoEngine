#include "GameObject.h"
#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(glm::vec2 position, glm::vec2 size, Texture* texture)
    : position(position), size(size), texture(texture), currentAnimation(nullptr) {}

GameObject::~GameObject() {}

void GameObject::Update(float deltaTime) {
  if (currentAnimation) {
    currentAnimation->Update(deltaTime);
  }
}

void GameObject::Draw(const glm::mat4& view, const glm::mat4& projection,
                      GLuint shaderProgram, GLuint VAO, int indexCount,
                      bool useColor, glm::vec4 color) {
  // Use shader program
  glUseProgram(shaderProgram);

  // Create model matrix
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
  model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

  // Set uniforms
  unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Set color or texture
  unsigned int useColorLoc = glGetUniformLocation(shaderProgram, "useColor");
  glUniform1i(useColorLoc, useColor ? 1 : 0);

  if (useColor) {
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLoc, color.r, color.g, color.b, color.a);
    // Set default texture offset/scale for colored objects
    unsigned int textureOffsetScaleLoc = glGetUniformLocation(shaderProgram, "textureOffsetScale");
    glUniform4f(textureOffsetScaleLoc, 0.0f, 0.0f, 1.0f, 1.0f);
  } else if (texture) {
    texture->Bind();
    
    // Set texture offset and scale for animation
    glm::vec4 textureOffsetScale(0.0f, 0.0f, 1.0f, 1.0f); // Default: full texture
    if (currentAnimation) {
      textureOffsetScale = currentAnimation->GetCurrentFrameCoords();
    }
    unsigned int textureOffsetScaleLoc = glGetUniformLocation(shaderProgram, "textureOffsetScale");
    glUniform4f(textureOffsetScaleLoc, textureOffsetScale.x, textureOffsetScale.y, 
                textureOffsetScale.z, textureOffsetScale.w);
  }

  // Draw
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

glm::vec4 GameObject::GetBoundingBox() const {
  // Returns (x, y, width, height)
  return glm::vec4(position.x, position.y, size.x, size.y);
}

