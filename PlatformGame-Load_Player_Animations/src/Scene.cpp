#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/maps/", "bakgorundRemaster");
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
	
	leControle = Engine::GetInstance().textures->Load("Assets/Textures/Controls.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	Engine::GetInstance().render.get()->camera.x = 250 - player->position.getX()*2;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		verTexture = !verTexture;
	}
	
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (verTexture && leControle != nullptr) {
		int width, height;
		Engine::GetInstance().textures->GetSize(leControle, width, height);
		int windowWidth, windowHeight;
		Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

		SDL_Rect dstRect = { windowWidth - width - 10, 10, width, height };

		SDL_RenderCopy(Engine::GetInstance().render->renderer, leControle, nullptr, &dstRect);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);
	if (leControle != nullptr) {
		Engine::GetInstance().textures->UnLoad(leControle);
		leControle = nullptr;
	}

	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();

}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");


	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);

	pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");

	//enemies
	size_t enemyIndex = 0;
	for (pugi::xml_node enemyNode = enemiesNode.child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy")) {
		// Read the enemy's position and attributes
		Vector2D enemyPos(
			enemyNode.attribute("x").as_int(),
			enemyNode.attribute("y").as_int()
		);

		//// If the EnemyList has enough enemies to update
		//if (enemyIndex < enemyList.size()) {
		//	// Update the position of the existing enemy in the list
		//	enemyList[enemyIndex].SetPosition(enemyPos);

		//	// Optionally, load other attributes like texture, gravity, etc.
		//	std::string texturePath = enemyNode.attribute("texture").as_string();
		//	bool gravity = enemyNode.attribute("gravity").as_bool();

		//	// Assuming Enemy class has methods to set texture and gravity
		//	enemyList[enemyIndex].SetTexture(texturePath);
		//	enemyList[enemyIndex].SetGravity(gravity);
		//}
		//else {
		//	LOG("Warning: More enemies in XML than in EnemyList.");
		//	break;
		//}

		// Increment the index to update the next enemy in the list
		++enemyIndex;
	}

}

void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	//enemies
	

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}