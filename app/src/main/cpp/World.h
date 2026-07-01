#ifndef PUPPY_BIRD_WORLD_H
#define PUPPY_BIRD_WORLD_H

class Player;
class Background;
class Building;
class Bird;
class Bomber;
class Fighter;
class Meteor;


class IEntity;

#include <vector>

class World
{
private:
    Background* background;

    std::vector<IObject*> objects;
    std::vector<IEntity*> entities;
};


#endif //PUPPY_BIRD_WORLD_H
