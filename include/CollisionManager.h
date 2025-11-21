#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "GameObject.h"
#include <glm/glm.hpp>

class CollisionManager {
public:
  static bool CheckCollision(const GameObject& obj1, const GameObject& obj2);
};

#endif // COLLISIONMANAGER_H

