#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;

class Checkpoint : public Entity
{
public:

	Checkpoint();

	virtual ~Checkpoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the Checkpoint. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	void Respawn();

	Vector2D GetPosition();

public:

	//Declare Checkpoint parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the Checkpoint - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 1.75f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the Checkpoint is currently jumping
	bool isFalling = false;
	bool GodMode = false;
	bool isFlipeado = false;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation run;
	Animation jump;
	Animation fall;

	SDL_RendererFlip flipea = SDL_FLIP_NONE;
};