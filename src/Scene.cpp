#include "Scene.h"

Scene::Scene() {}

Scene::~Scene() {
  Cleanup();
}

void Scene::AddGameObject(GameObject* obj) {
  if (obj) {
    m_GameObjects.push_back(obj);
  }
}

GameObject* Scene::GetGameObject(size_t index) {
  if (index < m_GameObjects.size()) {
    return m_GameObjects[index];
  }
  return nullptr;
}

void Scene::UpdatePlayerMovement(glm::vec2 movement, float speed, float deltaTime, bool& wasColliding) {
  if (m_GameObjects.size() == 0) return;
  
  GameObject* player = m_GameObjects[0];
  glm::vec2 nextPosition = player->position + movement * speed * deltaTime;
  
  // Check collision with potential new position
  GameObject tempPlayer = *player;
  tempPlayer.position = nextPosition;
  
  bool canMoveX = true;
  bool canMoveY = true;
  bool isColliding = false;
  
  // Check collision with all other objects (skip player itself and reference point)
  for (size_t i = 2; i < m_GameObjects.size(); i++) {
    GameObject* other = m_GameObjects[i];
    
    // Check X-axis collision
    GameObject tempX = tempPlayer;
    tempX.position.x = nextPosition.x;
    tempX.position.y = player->position.y;
    if (CollisionManager::CheckCollision(tempX, *other)) {
      canMoveX = false;
      isColliding = true;
    }
    
    // Check Y-axis collision
    GameObject tempY = tempPlayer;
    tempY.position.x = player->position.x;
    tempY.position.y = nextPosition.y;
    if (CollisionManager::CheckCollision(tempY, *other)) {
      canMoveY = false;
      isColliding = true;
    }
  }
  
  // Update position only if no collision
  if (canMoveX) {
    player->position.x = nextPosition.x;
  }
  if (canMoveY) {
    player->position.y = nextPosition.y;
  }
  
  wasColliding = isColliding;
}

void Scene::CheckCollisions(GameObject* player, bool& isColliding) {
  isColliding = false;
  if (!player) return;
  
  for (size_t i = 2; i < m_GameObjects.size(); i++) {
    if (CollisionManager::CheckCollision(*player, *m_GameObjects[i])) {
      isColliding = true;
      break;
    }
  }
}

void Scene::Cleanup() {
  for (GameObject* obj : m_GameObjects) {
    delete obj;
  }
  m_GameObjects.clear();
}

