#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();
	
	void Move(float dt);

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	std::string GetName();
public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	std::string name;
	int texW, texH;
	bool gravity;
	float speed;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	PhysBody* pbody;
	Pathfinding* pathfinding;
	bool debug = false;
	int maxFrameDuration = 16;
	bool flip = false;
	SDL_RendererFlip hflip = SDL_FLIP_NONE;
	bool alive;
};
