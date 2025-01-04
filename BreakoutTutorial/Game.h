/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include "GameLevel.h"
#include "SpriteRenderer.h"

#include <tuple>


enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

struct Collision
{
    bool HasCollided;
    Direction CollisionDirection;
    glm::vec2 CollisionDifference;

};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	SpriteRenderer* Renderer;

    std::vector<GameLevel> Levels;
    unsigned int CurrentLevel;

    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void InitBall();
    void InitPlayer();
    void ResetLevel();
    void ResetPlayer();
};