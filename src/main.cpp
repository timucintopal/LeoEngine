#include "Game.h"

Game *game = nullptr;

int main(int argc, char *argv[]) {
  game = new Game();

  game->Init("Wayne Engine", 800, 600, false);

  while (game->Running()) {
    game->HandleEvents();
    game->Update();
    game->Render();
  }

  game->Clean();
  delete game;

  return 0;
}
