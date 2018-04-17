#include "NativeWrapper.h"

//Game.h
extern "C" TERRAIN_EXPORT Game* newGameObject()
{
	return new Game();
}

extern "C" TERRAIN_EXPORT void deleteGameObject(Game* instance)
{
	delete instance;
}

extern "C" TERRAIN_EXPORT void InitGame(Game* instance)
{
	instance->Init();
}

extern "C" TERRAIN_EXPORT void UpdateGame(Game* instance)
{
	instance->Update();
}