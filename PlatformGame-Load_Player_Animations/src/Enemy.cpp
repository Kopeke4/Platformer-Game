#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Checkpoint.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	gravity = parameters.attribute("gravity").as_bool();
	speed = parameters.attribute("speed").as_float();
	name = parameters.attribute("name").as_string();
	alive = parameters.attribute("alive").as_bool();
	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Enemy::Update(float dt)
{
	alive = parameters.attribute("alive").as_bool();
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
	}
	ResetPath();

	while (pathfinding->pathTiles.empty()) 
	{
		pathfinding->PropagateAStar(SQUARED);
	}

	Move(dt);

	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	// Draw pathfinding 
	if (debug) {

	pathfinding->DrawPath();
	}

	return true;

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	// Move left
	if (velocity.x > 0) {
		flip = true;
		if (flip == true && hflip == SDL_FLIP_NONE)
		{
			hflip = SDL_FLIP_HORIZONTAL;
		}
	}

	// Move right
	if ( 0 > velocity.x) {
		flip = false;
		if (flip == false && hflip == SDL_FLIP_HORIZONTAL)
		{
			hflip = SDL_FLIP_NONE;
		}
	}
}

void Enemy::Move(float dt)
{
	if(pathfinding->pathTiles.size() > 3&& !(pathfinding->pathTiles.size() > 18)) {

		auto it = pathfinding->pathTiles.end();  // Start at the end of the path
		std::advance(it, -4);
		Vector2D targetTile = *it;
		Vector2D targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());

		//Calculate vector movement
		Vector2D movement = targetWorldPos + Vector2D(16, 16) - GetPosition();
		float distance = movement.magnitude();

		movement.normalized();
		b2Vec2 velocity(movement.getX() * speed, movement.getY() * speed);

		if (distance > 1.0f) {
			if (!gravity) { //If distance is bigger than
				pbody->body->SetLinearVelocity(velocity);
			}
			else {
				velocity.y = pbody->body->GetLinearVelocity().y;
				pbody->body->SetLinearVelocity(velocity);

			}
		}
	}
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

std::string Enemy::GetName() {
	return name;
}
