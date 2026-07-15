#ifndef PUPPY_BIRD_WORLD_H
#define PUPPY_BIRD_WORLD_H

#include "Entity.h"
#include <vector>

class World
{
public:
    Background background;
    Background secondBackground; // немного костыль, нужный для создания эффекта движения в пространстве
    Player player;
    Meteor meteor;

    std::vector<Bird> birds;
    std::vector<Bomber> bombers;
    std::vector<Fighter> fighters;

    std::vector<Building> buildings;
    std::vector<Terrain> terrainPipeline;

    std::vector<Bullet> playerBullets;
    std::vector<Bullet> enemyBullets;

    float borderX = 0.0f;
    float borderY = 0.0f;
};

#endif //PUPPY_BIRD_WORLD_H
