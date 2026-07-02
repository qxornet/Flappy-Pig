#ifndef FLAPPY_PIG_SYSTEMS_H
#define FLAPPY_PIG_SYSTEMS_H

#include "World.h"

class ISystem
{
public:
    virtual ~ISystem() = default;
    virtual void update(float dt, World& world) = 0;
};

class MovementSystem : public ISystem
{
    void update(float dt, World& world) override;
};

class CollisionSystem : public ISystem
{
    void update(float dt, World& world) override;
};

class InputSystem : public ISystem
{
    void update(float dt, World& world) override;
};

class HealthSystem : public ISystem
{
    void update(float dt, World& world) override;
};

class SpawnSystem : public ISystem
{
    // данный класс отвечает скорее не за создание entity, а за reset компонентов и свойств
    void update(float dt, World& world) override;
};

class AnimationSystem : public ISystem
{
    // проверить условия задания определенной текстуры, если не выполнено, то вызвать смену текстуры раз в switch time на текущем behaivor, данные брать из AnimationComponent
    // под "условиямм задания определенной текстуры" понимаются энкаунтеры или смерть actor'а
    void update(float dt, World& world) override;
};

class EventSystem : public ISystem
{
    void update(float dt, World& world) override;
};

class UltimateSystem : public ISystem
{
    void update(float dt, World& world) override;
};

#endif //FLAPPY_PIG_SYSTEMS_H
