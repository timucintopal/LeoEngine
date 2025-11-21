#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec2 initialPosition, int screenWidth, int screenHeight)
    : position(initialPosition), m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight) {}

glm::mat4 Camera::GetViewMatrix() const {
  // Calculate view matrix to keep target at screen center
  // Screen center is at (screenWidth/2, screenHeight/2)
  float screenCenterX = m_ScreenWidth / 2.0f;
  float screenCenterY = m_ScreenHeight / 2.0f;
  
  return glm::translate(glm::mat4(1.0f), 
                        glm::vec3(-position.x + screenCenterX, 
                                 -position.y + screenCenterY, 0.0f));
}

glm::mat4 Camera::GetProjectionMatrix(int screenWidth, int screenHeight) const {
  return glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
}

void Camera::Follow(const glm::vec2& targetPosition, float deltaTime) {
  // For simplicity, just set position to target position
  // Can be extended with interpolation or smoothing later
  position = targetPosition;
}

