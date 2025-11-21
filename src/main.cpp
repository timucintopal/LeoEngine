#include "Game.h"

Game *game = nullptr;

int main(int argc, char *argv[]) {
  game = new Game();

  game->Init("Wayne Engine", 800, 600, false);

  Uint64 lastTime = SDL_GetTicks64();
  Uint64 currentTime;
  float deltaTime;

  while (game->Running()) {
    currentTime = SDL_GetTicks64();
    deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    game->HandleEvents();
    game->Update(deltaTime);
    game->Render();
  }

  game->Clean();
  delete game;

  return 0;
}
