#ifndef PUPPY_BIRD_WORLD_H
#define PUPPY_BIRD_WORLD_H

#include "Entity.h"
#include <vector>

class World
{
public:
    Background background;
    Player player;
    Meteor meteor;

    std::vector<Bird> birds;
    std::vector<Bomber> bombers;
    std::vector<Fighter> fighters;

    std::vector<Building> buildings;
    std::vector<Terrain> terrainPipeline;

    float borderX = 0.0f;
    float borderY = 0.0f;
};

#endif //PUPPY_BIRD_WORLD_H
