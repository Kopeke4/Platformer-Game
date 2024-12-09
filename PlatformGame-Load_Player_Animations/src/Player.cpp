#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Checkpoint.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(0, 0);
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	run.LoadAnimations(parameters.child("animations").child("run"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	currentAnimation = &idle;

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	if (velocity.x == 0)
	{
		currentAnimation = &idle;
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		Attack();
	}
	// Move left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
		isFlipeado = true;
		currentAnimation = &run;
		if (isFlipeado == true && flipea == SDL_FLIP_NONE)
		{
			flipea = SDL_FLIP_HORIZONTAL;
		}
	}

	// Move right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
		isFlipeado = false;   
		currentAnimation = &run;
		if (isFlipeado == false && flipea == SDL_FLIP_HORIZONTAL)
		{
			flipea = SDL_FLIP_NONE;
		}
	}
	
	//Jump
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		// Apply an initial upward force
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
		isJumping = true;
		
	}

	//Jump
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && GodMode == true) {
		// Apply an initial upward force
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.1), true);
		isJumping = true;

	}

	if (isJumping == true) currentAnimation = &jump;

	if (velocity.y > 1) currentAnimation = &fall;


	// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
	if(isJumping == true)
	{
		velocity.y = pbody->body->GetLinearVelocity().y;
	}

	//God Mode
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		GodMode = !GodMode;
		if (GodMode) {
			LOG("God Mode activated");
		}
		else {
			LOG("God Mode deactivated");
		}
	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), flipea);
	currentAnimation->Update();

	

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PINCHO:
		LOG("Collision PINCHO");
		Respawn(); 
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::ENEMY:
		Respawn();
	default:
		break;
	}
}

void Player::Respawn() {

	pugi::xml_document saveFile;

	
	if (!saveFile.load_file("save.xml")) {
		std::cerr << "Error: No se pudo cargar el archivo save.xml" << std::endl;
		return;
	}

	pugi::xml_node playerNode = saveFile.child("config")
		.child("scene")
		.child("entities")
		.child("player");
	if (!playerNode) {
		std::cerr << "Error: No se encontró el nodo <player> en el archivo save.xml" << std::endl;
		return;
	}

	//Vector2D playerPos = Vector2D(playerNode.attribute("x").as_int(),
	//	playerNode.attribute("y").as_int());
	//SetPosition(playerPos);

}


void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Player::Attack()
{
	//// Define the attack hitbox dimensions
	//int attackWidth = 32;  // Width of the attack area
	//int attackHeight = texH;  // Same height as the player sprite

	//// Determine attack position based on player position and facing direction
	//int attackX = position.getX() + (pbody->body->GetLinearVelocity().x >= 0 ? texW : -attackWidth);
	//int attackY = position.getY();

	//// Create the attack rectangle
	//SDL_Rect attackRect = { attackX, attackY, attackWidth, attackHeight };

	// //Check collision with enemies
	//for (auto enemy : Engine::GetInstance().scene.get()->GetEnemies())
	//{
	//	// Get enemy's current position and hitbox size
	//	Vector2D enemyPos = enemy->GetPosition();
	//	SDL_Rect enemyRect = { (int)enemyPos.getX(), (int)enemyPos.getY(), 32, 32 };

	//	// Detect overlap
	//	if (SDL_HasIntersection(&attackRect, &enemyRect)) 
	//	{
	//		// Kill the enemy by calling DestroyEntity through the entity manager
	//		pugi::xml_document loadFile;
	//		pugi::xml_parse_result result = loadFile.load_file("config.xml");
	//		pugi::xml_node sceneNode = loadFile.child("config").child("scene");
	//		sceneNode.child("entities").child("enemies").child("enemy").child(enemy->GetName()).attribute("alive").set_value(false);
	//	}
	//}
	
}