#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL.h>
#include <glm/glm.hpp>

class InputManager {
public:
  InputManager();
  ~InputManager();

  void Update();
  bool IsKeyPressed(SDL_Scancode key) const;
  bool IsKeyJustPressed(SDL_Scancode key) const;
  glm::vec2 GetMovementInput() const;

private:
  const Uint8* m_CurrentState;
  Uint8* m_PreviousState;
  int m_NumKeys;
};

#endif // INPUTMANAGER_H

