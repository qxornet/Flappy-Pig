#ifndef FLAPPY_PIG_SYSTEMS_H
#define FLAPPY_PIG_SYSTEMS_H

#include "World.h"

class ISystem
{
public:
    virtual ~ISystem() = default;
    virtual void update(float dt, World& world, GameState &state) = 0;
};

class MovementSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class CollisionSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class InputSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class HealthSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class SpawnSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class AnimationSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class EventSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

class UltimateSystem : public ISystem
{
    void update(float dt, World& world, GameState &state) override;
};

#endif //FLAPPY_PIG_SYSTEMS_H
