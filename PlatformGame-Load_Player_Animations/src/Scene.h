#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include "Checkpoint.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Vector2D GetPlayerPosition();

	void LoadState();
	void SaveState();

	std::vector<Enemy*> GetEnemies() const { return enemyList; };

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}


private:
	SDL_Texture* img;
	bool verTexture = false;
	std::string tilePosDebug = "[0,0]";
	SDL_Texture* leControle;
	SDL_Texture* title = nullptr;
	int BackgroundMusic;
	bool isPaused = false;
	bool IntroScreen = false;
	int level = 0;
	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;

};