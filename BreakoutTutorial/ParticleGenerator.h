#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "GameObject.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
	public:
	    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
		void Update(float dt, GameObject& parentObject, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
		void Draw();

	private:
	    unsigned int amount = 500;
		unsigned int lastUsedParticle = 0;
	    std::vector<Particle> particles;
		Shader particleShader;
		Texture2D particleTexture;
		unsigned int particleVAO;

		void init();

	    unsigned int FirstUnusedParticle();
	    void RespawnParticle(Particle& particle, GameObject& parentObject, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};
