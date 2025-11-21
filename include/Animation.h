#ifndef ANIMATION_H
#define ANIMATION_H

#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>

class Animation {
public:
  Animation(Texture* spriteSheet, const std::vector<glm::vec4>& frames, float frameDuration);
  ~Animation();

  void Update(float deltaTime);
  glm::vec4 GetCurrentFrameCoords() const;
  void Reset();

private:
  Texture* m_SpriteSheet;
  std::vector<glm::vec4> m_Frames; // (x, y, width, height) in normalized texture coordinates
  float m_FrameDuration;
  int m_CurrentFrameIndex;
  float m_Timer;
};

#endif // ANIMATION_H

