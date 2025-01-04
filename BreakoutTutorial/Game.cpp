#include "Game.h"

#include <algorithm>
#include <GLFW/glfw3.h>

#include "ResourceManager.h"
#include "SpriteRenderer.h"

// player stuff
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

// ball stuff
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height){

}

Game::~Game() {
    delete Renderer;
    delete Player;
    delete Ball;
}

void Game::Init() {
    // load shaders
    LoadShaders();

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    LoadTextures();

    // load levels
    GameLevel one;
    one.Load("resources/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; 
    two.Load("resources/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three;
    three.Load("resources/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four;
    four.Load("resources/levels/four.lvl", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->CurrentLevel = 0;

    InitPlayer();
    InitBall();
    InitParticles();
}

void Game::LoadShaders() {
    ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particle.vert", "shaders/particle.frag", nullptr, "particle");
}

void Game::LoadTextures() {
    ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
}



void Game::Update(float dt) {
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    if(Ball->Position.y >= this->Height) {
        this->ResetLevel();
        this->ResetPlayer();
    }
    // update particle generator
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
}

void Game::ProcessInput(float dt) {
    if(this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        if(this->Keys[GLFW_KEY_A]) {
	        if(Player->Position.x > 0.0f) {
                Player->Position.x = std::max(Player->Position.x - velocity, 0.0f);
                if(Ball->Stuck) {
                    Ball->Position.x -= velocity;
                }
	        }
        }
        if(this->Keys[GLFW_KEY_D]) {
	        if(Player->Position.x < this->Width - Player->Size.x) {
                Player->Position.x = std::min(Player->Position.x + velocity, this->Width - Player->Size.x);
                if(Ball->Stuck) {
                    Ball->Position.x += velocity;
                }
	        }
        }
        if(this->Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false;
        }
    }
}

void Game::Render() {
    if(this->State == GAME_ACTIVE) {
		// draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw blocks / level
        this->Levels[this->CurrentLevel].Draw(*Renderer);
        // draw player
        Player->Draw(*Renderer);
        // draw particles
        Particles->Draw();
        // draw ball
    	Ball->Draw(*Renderer);
    }


	//Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

Direction VectorDirection(glm::vec2 target, float aspectRatio = 1.0f) {
    // we adjust the compass directions based on the aspect ratio, where we assume that up and down has a length of 1 and left/right is scaled accordingly.
    // The scaling should account for the "aspect ratio" of the box
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f * aspectRatio, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f * aspectRatio, 0.0f)	// left
    };

    float max = 0.0f;
    unsigned int bestMatch = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dotProduct = glm::dot(glm::normalize(target), compass[i]);
        if (dotProduct > max) {
            max = dotProduct;
            bestMatch = i;
        }
    }
    return (Direction)bestMatch;

}

//// AABB to AABB Collision
//bool CheckCollision(GameObject& first, GameObject& second) {
//
//	// check x-axis
//    bool collisionX = first.Position.x + first.Size.x >= second.Position.x && second.Position.x + second.Size.x >= first.Position.x;
//    // check y-axis
//    bool collisionY = first.Position.y + first.Size.y >= second.Position.y && second.Position.y + second.Size.y >= first.Position.y;
//
//	// collision only if both overlap
//    return collisionX && collisionY;
//}

// Circle to AABB collision
Collision CheckCollision(BallObject& ball, GameObject& box) {
    glm::vec2 ballCenter(ball.Position + ball.Radius);

	glm::vec2 boxHalfExtent(box.Size.x / 2.0f, box.Size.y / 2.0f);
    glm::vec2 boxCenter(box.Position.x + boxHalfExtent.x, box.Position.y + boxHalfExtent.y);

    glm::vec2 difference = ballCenter - boxCenter;
    glm::vec2 clamped = glm::clamp(difference, -boxHalfExtent, boxHalfExtent);

    // add clamped value to box center to find the point on the box that is closest to the circle's center
    glm::vec2 closest = boxCenter + clamped;
    difference = closest - ballCenter;

    // overlap if the circle is closer to the point on the box than the circle's radius
    if(glm::length(difference) <= ball.Radius) {
        float aspectRatio = box.Size.x / box.Size.y;
        return Collision{ true, VectorDirection(difference, aspectRatio), difference };
    }

    return Collision{ false, UP, glm::vec2(0.0f, 0.0f) };
}

void Game::DoCollisions() {
    for(GameObject& box : this->Levels[this->CurrentLevel].Bricks) {
        if (box.Destroyed) {
            continue;
        }

    	Collision collision = CheckCollision(*Ball, box);

    	if (!collision.HasCollided) {
            continue;
        }

        if(!box.IsSolid) {
            box.Destroyed = true;
        }

        // collision reaction
        Direction dir = collision.CollisionDirection;
        glm::vec2 diffVector = collision.CollisionDifference;
        // horizontal collision
        if(dir == LEFT || dir == RIGHT) {
            float penetration = Ball->Radius - std::abs(diffVector.x);
            if(dir == LEFT) {
                // only flip velocity if it has not yet been flipped
                if (Ball->Velocity.x < 0) {
                    Ball->Velocity.x *= -1;
                }
                Ball->Position.x += penetration;
            }
            else {
                // only flip velocity if it has not yet been flipped
                if (Ball->Velocity.x > 0) {
                    Ball->Velocity.x *= -1;
                }
                Ball->Position.x -= penetration;
            }
        }
        // vertical collision
        else {
            Ball->Velocity.y *= -1;
            float penetration = Ball->Radius - std::abs(diffVector.y);
            if(dir == UP) {
                // only flip velocity if it has not yet been flipped
                if (Ball->Velocity.y > 0) {
                    Ball->Velocity.y *= -1;
                }
                Ball->Position.y -= penetration;
            }
            else {
                // only flip velocity if it has not yet been flipped
                if (Ball->Velocity.y < 0) {
                    Ball->Velocity.y *= -1;
                }
                Ball->Position.y += penetration;
            }
        }
    }

    // check for collision with the player
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && result.HasCollided) {
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);

    	float strength = 2.0f;

        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

void Game::InitPlayer() {
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}


void Game::InitBall() {
    glm::vec2 ballPos = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::InitParticles() {
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 10000);
}

void Game::ResetLevel() {
    switch(this->CurrentLevel) {
	    case 0: this->Levels[0].Load("resources/levels/one.lvl", this->Width, this->Height / 2); break;
	    case 1: this->Levels[1].Load("resources/levels/two.lvl", this->Width, this->Height / 2); break;
	    case 2: this->Levels[2].Load("resources/levels/three.lvl", this->Width, this->Height / 2); break;
	    case 3: this->Levels[3].Load("resources/levels/four.lvl", this->Width, this->Height / 2); break;
    }
    //if (this->CurrentLevel == 0)
    //    this->Levels[0].Load("rresources/levels/one.lvl", this->Width, this->Height / 2);
    //else if (this->CurrentLevel == 1)
    //    this->Levels[1].Load("resources/levels/two.lvl", this->Width, this->Height / 2);
    //else if (this->CurrentLevel == 2)
    //    this->Levels[2].Load("resources/levels/three.lvl", this->Width, this->Height / 2);
    //else if (this->CurrentLevel == 3)
    //    this->Levels[3].Load("resources/levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer() {
    delete Player;
    InitPlayer();
    delete Ball;
    InitBall();
}
