#include "Animation.h"

Animation::Animation(Texture* spriteSheet, const std::vector<glm::vec4>& frames, float frameDuration)
    : m_SpriteSheet(spriteSheet), m_Frames(frames), m_FrameDuration(frameDuration),
      m_CurrentFrameIndex(0), m_Timer(0.0f) {
  if (m_Frames.empty()) {
    m_FrameDuration = 0.0f;
  }
}

Animation::~Animation() {
  // Don't delete m_SpriteSheet, it's managed elsewhere
}

void Animation::Update(float deltaTime) {
  if (m_Frames.empty() || m_FrameDuration <= 0.0f) {
    return;
  }

  m_Timer += deltaTime;

  while (m_Timer >= m_FrameDuration) {
    m_Timer -= m_FrameDuration;
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Frames.size();
  }
}

glm::vec4 Animation::GetCurrentFrameCoords() const {
  if (m_Frames.empty()) {
    return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Default: full texture
  }
  return m_Frames[m_CurrentFrameIndex];
}

void Animation::Reset() {
  m_CurrentFrameIndex = 0;
  m_Timer = 0.0f;
}

