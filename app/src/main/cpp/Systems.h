//
// Created by ilya on 01.07.2026.
//

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
    void update(float dt, World& world) override {}
};

class CollisionSystem : public ISystem
{
    void update(float dt, World& world) override {}
};

class InputSystem : public ISystem
{
    void update(float dt, World& world) override {}
};

class SpawnSystem : public ISystem
{
    // данный класс отвечает скорее не за создание entity, а скорее за частичный reset
    void update(float dt, World& world) override {}
};

class AnimationSystem : public ISystem
{
    // проверить условия задания определенной текстуры, если не выполнено, то вызвать смену текстуры раз в switch time на текущем behaivor, данные брать из AnimationComponent
    // под "условиямм задания определенной текстуры" понимаются энкаунтеры или смерть actor'а
    void update(float dt, World& world) override {}
};

class UltimateSystem : public ISystem
{
    // проверить наличие очков и нажатие кнопки Ultimate, если кнопка нажата - то все actor'ы на поле уничтожаются (all reset) (облегчение гейплея игрока)
    void update(float dt, World& world) override {}
};


#endif //FLAPPY_PIG_SYSTEMS_H
