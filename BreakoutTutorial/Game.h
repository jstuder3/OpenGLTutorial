/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include "BallObject.h"
#include "GameLevel.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "PowerUp.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"

enum Direction {
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

struct Collision {
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
    bool KeysProcessed[1024];
	unsigned int Width, Height;

    const unsigned int MAX_LIVES = 3;
    unsigned int Lives = MAX_LIVES;

	SpriteRenderer* Renderer;

    float ShakeTime = 0.0f;

    std::vector<GameLevel> Levels;
    unsigned int CurrentLevel;

    GameObject* Player;
    BallObject* Ball;
    ParticleGenerator* Particles;

    PostProcessor* PostProcess;

    TextRenderer* Text;

    std::vector<PowerUp> PowerUps;

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
    void InitParticles();

	void ResetLevel();
    void ResetPlayer();

    void LoadShaders();
    void LoadTextures();

    void SpawnPowerUps(GameObject& block);
    void UpdatePowerUps(float dt);
	void ActivatePowerUp(PowerUp& powerUp);
};