#pragma once

#include "SpriteRenderer.h"

// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	SpriteRenderer* Renderer;
	Game(unsigned int width, unsigned int height);
	~Game();

	void Init();

	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
};
