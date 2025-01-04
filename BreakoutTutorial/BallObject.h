#pragma once

#include "GameObject.h"
#include "Texture.h"

class BallObject : public GameObject
{
public:
	float Radius;
	bool Stuck;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int windowWidth);
	void Reset(glm::vec2 position, glm::vec2 velocity);
	
};
