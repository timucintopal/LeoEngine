#include "InputManager.h"
#include <cstring>

InputManager::InputManager() 
    : m_CurrentState(nullptr), m_PreviousState(nullptr), m_NumKeys(0) {
  m_CurrentState = SDL_GetKeyboardState(&m_NumKeys);
  m_PreviousState = new Uint8[m_NumKeys];
  std::memset(m_PreviousState, 0, m_NumKeys);
}

InputManager::~InputManager() {
  if (m_PreviousState) {
    delete[] m_PreviousState;
    m_PreviousState = nullptr;
  }
}

void InputManager::Update() {
  if (m_PreviousState && m_CurrentState) {
    std::memcpy(m_PreviousState, m_CurrentState, m_NumKeys);
  }
  m_CurrentState = SDL_GetKeyboardState(&m_NumKeys);
}

bool InputManager::IsKeyPressed(SDL_Scancode key) const {
  if (!m_CurrentState) return false;
  return m_CurrentState[key] != 0;
}

bool InputManager::IsKeyJustPressed(SDL_Scancode key) const {
  if (!m_CurrentState || !m_PreviousState) return false;
  return m_CurrentState[key] != 0 && m_PreviousState[key] == 0;
}

glm::vec2 InputManager::GetMovementInput() const {
  glm::vec2 movement(0.0f);
  if (IsKeyPressed(SDL_SCANCODE_W)) movement.y -= 1.0f;
  if (IsKeyPressed(SDL_SCANCODE_S)) movement.y += 1.0f;
  if (IsKeyPressed(SDL_SCANCODE_A)) movement.x -= 1.0f;
  if (IsKeyPressed(SDL_SCANCODE_D)) movement.x += 1.0f;
  return movement;
}

