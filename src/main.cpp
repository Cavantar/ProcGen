#include "Includes.h"
#include "Game.h"

int main(int argc, char **argv) {

  glutInit(&argc, argv);
  
  Game game;
  game.setupAndStart();
  
  return 0;
}
