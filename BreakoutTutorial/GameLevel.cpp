#include "GameLevel.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "ResourceManager.h"


void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight) {
	// remove old level
	this->Bricks.clear();
	// load from file
	// GameLevel level;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if(fstream) {
		std::string line;
		while(std::getline(fstream, line)) {
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			unsigned int tileCode;
			while(sstream >> tileCode) {
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		if(!tileData.empty()) {
			this->init(tileData, levelWidth, levelHeight);
		}
	}
	else {
		std::cerr << "ERROR: File " << file << " was not found in GameLevel::Load" << std::endl;
	}
}

void GameLevel::Draw(SpriteRenderer& renderer) {
	for (GameObject& tile : this->Bricks) {
		if(!tile.Destroyed) {
			tile.Draw(renderer);
		}
	}
}

bool GameLevel::IsCompleted() {
	for(GameObject& tile : this->Bricks) {
		if(!tile.IsSolid && !tile.Destroyed) {
			return false;
		}
	}
	return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) {
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unitWidth = levelWidth / static_cast<float>(width);
	float unitHeight = levelHeight / static_cast<float>(height);

	for(unsigned int row = 0; row < height; row++) {
		for(unsigned int col = 0; col < width; col++) {

			glm::vec2 pos(unitWidth * col, unitHeight * row);
			glm::vec2 size(unitWidth, unitHeight);

			// solid tile
			if(tileData[row][col] == 1) {
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[row][col] > 1) {
				glm::vec3 color = glm::vec3(1.0f);
				if(tileData[row][col] == 2) {
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				}
				else if(tileData[row][col] == 3) {
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				}
				else if(tileData[row][col] == 4) {
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				}
				else if(tileData[row][col] == 5) {
					color = glm::vec3(1.0f, 0.5f, 0.0f);
				}

				this->Bricks.emplace_back(pos, size, ResourceManager::GetTexture("block"), color);

			}
		}
	}

}


