#include "Game.h"

int main(int argc, char** argv)
{
	Game* game = new Game();

	game->Init();
	game->Update();

	return 0;
}