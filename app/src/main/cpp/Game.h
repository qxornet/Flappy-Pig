//
// Created by ilya on 26.06.2026.
//

#ifndef PUPPY_BIRD_GAME_H
#define PUPPY_BIRD_GAME_H

#include "Model.h"
#include "World.h"
#include "Renderer.h"
#include "Systems.h"

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
    void loadSystems();

private:
    android_app *application;
    AAssetManager *assetManager;

    Renderer renderer;
    World world;
    std::vector<Model> models;

    // упрощенный менеджер мешей
    std::map< EntityId, std::shared_ptr<MeshComponent> > meshStorage;
    std::vector<std::unique_ptr<ISystem>> systems;
};


#endif //PUPPY_BIRD_GAME_H
