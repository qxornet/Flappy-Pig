#ifndef PUPPY_BIRD_ENTITY_H
#define PUPPY_BIRD_ENTITY_H

#include "Components.h"


class IEntity
{
public:
    virtual ~IEntity() = default;

    TransformComponent transform;
    EntityId id;
};

class Renderable
{
public:
    Renderable() = default;
    Renderable(std::shared_ptr<TextureAsset> texture)
            : material(std::move(texture)) {
    }

    std::shared_ptr<MeshComponent> mesh;
    MaterialComponent material;
};

class Background : public IEntity, public Renderable
{
public:
    Background() {
        id = EntityId::eBackground;
    }
};

class Player : public IEntity, public Renderable
{
public:
    Player() {
        id = EntityId::ePlayer;
    }

    MovementComponent movement;
    HealthComponent health;
    //PlayerControllerComponent controller;
};

class Bird : public IEntity, public Renderable
{
public:
    Bird() {
        id = EntityId::eBird;
    }

    MovementComponent movement;
};

class Bomber : public IEntity, public Renderable
{
public:
    Bomber() {
        id = EntityId::eBomber;
    }

    MovementComponent movement;
    HealthComponent health;
};

class Bomb : public IEntity, public Renderable
{
public:
    Bomb() {
        id = EntityId::eBomb;
    }

    MovementComponent movement;
};

class Fighter : public IEntity, public Renderable
{
public:
    Fighter() {
        id = EntityId::eFighter;
    }

    MovementComponent movement;
    HealthComponent health;
    DamageComponent damage;
};

class Building : public IEntity, public Renderable
{
public:
    Building() {
        id = EntityId::eBuilding;
    }
    MovementComponent movement;
};

class Terrain : public IEntity, public Renderable
{
public:
    Terrain() {
        id = EntityId::eTerrain;
    }
};

class Meteor : public IEntity, public Renderable
{
public:
    Meteor() {
        id = EntityId::eMeteor;
    }
    MovementComponent movement;
};

#endif //PUPPY_BIRD_ENTITY_H
