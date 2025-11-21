#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(glm::vec2 position, glm::vec2 size, Texture* texture)
    : position(position), size(size), texture(texture) {}

GameObject::~GameObject() {}

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
  } else if (texture) {
    texture->Bind();
  }

  // Draw
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

