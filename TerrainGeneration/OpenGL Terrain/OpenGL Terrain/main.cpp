#include "Terrain.h"

int main()
{
	Terrain t = Terrain();

	//Load Texture stage 1 - grass
	t.LoadTexture("grass.jpg", 0);

	//Load Texture stage 2 - stone
	t.LoadTexture("stone.jpg", 1);

	//Load Texture stage 3 - snow
	t.LoadTexture("snow.jpg", 2);

	//Load the height map
	t.LoadHeightMap("tutorial.bmp", 32, 256, 256);

	t.Render();

	return 0;
}