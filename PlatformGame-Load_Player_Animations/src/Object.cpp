#include "Object.h"

Object::Object() : texture(nullptr), width(0), height(0), pbody(nullptr)
{
}

Object::~Object()
{
    CleanUp();
}

bool Object::Awake()
{
    // Inicialización necesaria
    return true;
}

bool Object::Start()
{
    // Código de inicio
    return true;
}

bool Object::Update(float dt)
{
    // Actualización del objeto
    return true;
}

bool Object::CleanUp()
{
    // Limpieza de recursos
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    return true;
}

void Object::SetPosition(Vector2D pos)
{
    position = pos;
}

Vector2D Object::GetPosition() const
{
    return position;
}
