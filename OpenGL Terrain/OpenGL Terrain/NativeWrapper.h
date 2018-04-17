#pragma once
#include "Game.h"

#ifndef TERRAIN_NATIVE_WRAPPER_H
#define TERRAIN_EXPORT __declspec(dllexport)

////////////////////////////////////////////////////////
//Game.h
////////////////////////////////////////////////////////
extern "C" TERRAIN_EXPORT Game* newGameObject();

extern "C" TERRAIN_EXPORT void deleteGameObject(Game* instance);

extern "C" TERRAIN_EXPORT void InitGame(Game* instance);

extern "C" TERRAIN_EXPORT void UpdateGame(Game* instance);

#endif