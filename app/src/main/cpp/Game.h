//
// Created by ilya on 26.06.2026.
//

#ifndef PUPPY_BIRD_GAME_H
#define PUPPY_BIRD_GAME_H

#include "Model.h"
#include "Renderer.h"

#include <map>

class Game
{
public:
    Game(android_app *app);

    void handleInput();
    void update(float dt);
    void render();

private:

    void loadMeshes();

private:
    android_app *app_;
    Renderer renderer;
    std::vector<Model> models;
    AAssetManager *assetManager;
//    World world;

    std::map< EntityId, std::shared_ptr<MeshComponent> > meshStorage;
};


#endif //PUPPY_BIRD_GAME_H
