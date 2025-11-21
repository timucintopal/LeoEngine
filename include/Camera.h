#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
  Camera(glm::vec2 initialPosition, int screenWidth, int screenHeight);
  
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix(int screenWidth, int screenHeight) const;
  
  void Follow(const glm::vec2& targetPosition, float deltaTime);
  
  glm::vec2 position;
  
private:
  int m_ScreenWidth;
  int m_ScreenHeight;
};

#endif // CAMERA_H

