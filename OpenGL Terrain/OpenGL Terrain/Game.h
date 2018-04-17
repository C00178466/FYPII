#pragma once
#include <iostream>
#include <stdlib.h>

#define GLUT_DISABLE_ATEXIT_HACK

#include "glut.h"
#include <Windows.h>
#include "Terrain.h"

using namespace std;

class Game
{
public:
	Game();
	~Game();

	void Init();
	void Update();

private:

};

void HandleInput(unsigned char key, int x, int y);
void CleanUp();
void InitRenderingFunctions();
void HandleResize(int w, int h);
void Render();
void UpdateRotation(int value);
