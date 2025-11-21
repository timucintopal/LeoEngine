#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"
#include "CollisionManager.h"
#include <vector>
#include <glm/glm.hpp>

class Scene {
public:
  Scene();
  ~Scene();

  void AddGameObject(GameObject* obj);
  GameObject* GetGameObject(size_t index);
  size_t GetGameObjectCount() const { return m_GameObjects.size(); }
  
  void UpdatePlayerMovement(glm::vec2 movement, float speed, float deltaTime, bool& wasColliding);
  void CheckCollisions(GameObject* player, bool& isColliding);

  void Cleanup();

private:
  std::vector<GameObject*> m_GameObjects;
};

#endif // SCENE_H

