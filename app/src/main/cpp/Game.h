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
    void loadTextures();
    void loadSystems();
    void createWorld();

private:
    android_app *application;
    AAssetManager *assetManager;

    Renderer renderer;
    World world;
    std::vector<Model> models;

    // упрощенный менеджер мешей
    std::unordered_map<EntityId, std::shared_ptr<MeshComponent>> meshStorage;
    std::unordered_map<EntityId, AnimationComponent> textureStorage;
    std::vector<std::unique_ptr<ISystem>> systems;
};

#endif //PUPPY_BIRD_GAME_H
