#ifndef PUPPY_BIRD_ENTITY_H
#define PUPPY_BIRD_ENTITY_H

#include "Components.h"

enum class EntityId
{
    eBackground,
    ePlayer,
    eBird,
    eBomber,
    eBomb,
    eFighter,
    eBuilding,
    eTerrain,
    eMeteor,
    eBullet,
    eEnemyBullet,
    ePlayerBullet,

    eUnknown
};

class IEntity
{
public:
    virtual ~IEntity() = default;

    TransformComponent transform;
    TransformComponent transform_default;
    EntityId id = EntityId::eUnknown;
    bool isEnable = false; // отвечает за взаимодействие Entity с окружающим миром
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
    MaterialComponent material_default;
    bool isShow = false; // отвечает за отображение Entity на сцене
};

class Digit : public IEntity, public Renderable
{
};

class Background : public IEntity, public Renderable
{
public:
    Background() {
        id = EntityId::eBackground;
    }

    MaterialComponent secondMaterial;
};

class Player : public IEntity, public Renderable
{
public:
    Player() {
        id = EntityId::ePlayer;
    }

    MovementComponent movement;
    HealthComponent health;
    ColliderComponent collision;

    MovementComponent movement_default;
    HealthComponent health_default;
    AnimationComponent animation;
    AttackCompoment attack;
    GamePointsComponent score;

    PlayerControllerComponent controller;

    int gamePoints = 0;
};

class Bird : public IEntity, public Renderable
{
public:
    Bird() {
        id = EntityId::eBird;
    }

    HealthComponent health;
    MovementComponent movement;
    HealthComponent health_default;
    MovementComponent movement_default;
    ColliderComponent collision;
    AnimationComponent animation;
    SpawnComponent spawn;
};

class Bomb : public IEntity, public Renderable
{
public:
    Bomb() {
        id = EntityId::eBomb;
    }

    MovementComponent movement;
    ColliderComponent collision;
    bool hasDestroy = false;
};

class Bomber : public IEntity, public Renderable
{
public:
    Bomber() {
        id = EntityId::eBomber;
    }

    ColliderComponent collision;
    AnimationComponent animation;
    MovementComponent movement;
    HealthComponent health;
    MovementComponent movement_default;
    HealthComponent health_default;
    SpawnComponent spawn;
    GamePointsComponent cost;

    Bomb bomb;
};


class Fighter : public IEntity, public Renderable
{
public:
    Fighter() {
        id = EntityId::eFighter;
    }

    ColliderComponent collision;
    AnimationComponent animation;
    MovementComponent movement;
    HealthComponent health;
    HealthComponent health_default;
    MovementComponent movement_default;
    AttackCompoment attack;
    SpawnComponent spawn;
    GamePointsComponent cost;
};

class Bullet : public IEntity, public Renderable
{
public:
    Bullet() {
        id = EntityId::eBullet;
    }

    ColliderComponent collision;
    AnimationComponent animation;
    MovementComponent movement;
};

class Building : public IEntity, public Renderable
{
public:
    Building() {
        id = EntityId::eBuilding;
    }
    MovementComponent movement;
    MovementComponent movement_default;
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

    ColliderComponent collision;
    AnimationComponent animation;
    EventComponent event;
    MovementComponent movement;
    EventComponent event_default;
    MovementComponent movement_default;
};

#endif //PUPPY_BIRD_ENTITY_H
