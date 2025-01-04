#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount) : particleShader(shader), particleTexture(texture), amount(amount), particles(amount) {
    this->init();
}

void ParticleGenerator::init() {
    unsigned int VBO;
    float particleQuad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->particleVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

unsigned int ParticleGenerator::FirstUnusedParticle() {
    // start checking from most recent particle
	for(unsigned int i = lastUsedParticle; i < amount; i++) {
		if(particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
		}
	}
    // otherwise iterate over all particles
    for(unsigned int i = 0; i < lastUsedParticle; i++) {
	    if(particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
	    }
    }
    // override first particle if all others are alive
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, GameObject& parentObject, glm::vec2 offset) {
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = parentObject.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 10.0f;
    particle.Velocity = parentObject.Velocity * 0.1f;
}

void ParticleGenerator::Update(float dt, GameObject& parentObject, unsigned int newParticles, glm::vec2 offset) {
    // spawn new particles
    for (unsigned int i = 0; i < newParticles; i++) {
        int unusedParticle = FirstUnusedParticle();
        RespawnParticle(particles[unusedParticle], parentObject, offset);
    }

    // update existing particles
    for (unsigned int i = 0; i < amount; i++) {
        Particle& p = particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f) {
            // particle is alive
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::Draw() {
    // set alpha blending to additive to get a "glow" effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    particleShader.Use();
    for(Particle& particle : particles) {
	    if(particle.Life > 0.0f) {
            particleShader.SetVector2f("offset", particle.Position);
            particleShader.SetVector4f("color", particle.Color);
            particleTexture.Bind();
            glBindVertexArray(particleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
	    }
    }
    // reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
