#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <time.h>
using namespace std;
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
int arr_size = pow(2.0, 11.0) + 1;
int terrain_array[10000];
float roughness = 0.45;
int intial_displacement = 100;
int intCount = 0;

void initializeArray()
{
	for (int i = 0; i < arr_size; i++)
	{
		terrain_array[i] = 0;
	}
}
void generateTerrain(int start, int end, int displacement)
{
	if ((start + 1) == end)
	{
		return;
	}
		
	int midindex = floor((start + end) / 2); //find mid point of line
	int change = ((rand() % 11) - 5) * displacement; //randomize a value to displace
	terrain_array[midindex] = (terrain_array[start] + terrain_array[end]) / 2 + change; //apply that value to the array
	displacement = displacement * roughness; //add roughness to the displacement
	generateTerrain(start, midindex, displacement);
	generateTerrain(midindex, end, displacement);
}
void drawTerrain()
{
	int start_point = 1;
	int i;
	SDL_SetRenderDrawColor(gRenderer, 96, 96, 255, 255);
	for (i = 1; i < arr_size; i++)
	{
		SDL_RenderDrawLine(gRenderer, start_point, SCREEN_HEIGHT, i, (terrain_array[(intCount + i) % arr_size] + 3 * intial_displacement));
		start_point = i;
	}
	intCount++;
}
int main()
{
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	gWindow = SDL_CreateWindow("Midpoint_Displacement_Algorithm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(gRenderer, 96, 96, 96, 96);

	initializeArray();
	generateTerrain(0, arr_size - 1, intial_displacement);

	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
			
		SDL_SetRenderDrawColor(gRenderer, 96, 96, 96, 96);
		SDL_RenderClear(gRenderer);
		drawTerrain();
		SDL_RenderPresent(gRenderer);
	}
	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();

	return 0;
}