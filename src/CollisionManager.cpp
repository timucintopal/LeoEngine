#include "CollisionManager.h"

bool CollisionManager::CheckCollision(const GameObject& obj1, const GameObject& obj2) {
  // AABB Collision Detection
  // GameObject position is center-based, so we need to convert to corner-based
  // Calculate left, right, top, bottom for each object
  
  float obj1Left = obj1.position.x - obj1.size.x / 2.0f;
  float obj1Right = obj1.position.x + obj1.size.x / 2.0f;
  float obj1Top = obj1.position.y - obj1.size.y / 2.0f;
  float obj1Bottom = obj1.position.y + obj1.size.y / 2.0f;
  
  float obj2Left = obj2.position.x - obj2.size.x / 2.0f;
  float obj2Right = obj2.position.x + obj2.size.x / 2.0f;
  float obj2Top = obj2.position.y - obj2.size.y / 2.0f;
  float obj2Bottom = obj2.position.y + obj2.size.y / 2.0f;
  
  // Check if bounding boxes overlap
  bool collisionX = obj1Left < obj2Right && obj1Right > obj2Left;
  bool collisionY = obj1Top < obj2Bottom && obj1Bottom > obj2Top;
  
  return collisionX && collisionY;
}

