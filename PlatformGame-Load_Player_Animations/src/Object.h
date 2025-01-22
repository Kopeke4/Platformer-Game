#pragma once

#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Vector2D.h"
#include "Physics.h"

class Object
{
public:
    Object();
    virtual ~Object();

    virtual bool Awake();
    virtual bool Start();
    virtual bool Update(float dt);
    virtual bool CleanUp();

    void SetPosition(Vector2D pos);
    Vector2D GetPosition() const;

protected:
    Vector2D position;
    SDL_Texture* texture;
    int width, height;
    PhysBody* pbody;
};

