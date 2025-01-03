#include "Game.h"

#include "ResourceManager.h"

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
	
}

Game::~Game() {
	delete Renderer;
}

void Game::Init() {
	// compile shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");

	// prepare projection matrix
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

	// set uniforms
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// load textures
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");

}

void Game::Update(float dt) {
	
}

void Game::ProcessInput(float dt) {
	
}

void Game::Render() {
	Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(0.0f, 0.0f), glm::vec2(800.f, 600.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

