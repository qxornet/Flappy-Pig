#ifndef PUPPY_BIRD_GAME_H
#define PUPPY_BIRD_GAME_H

#include "World.h"
#include "Renderer.h"
#include "Systems.h"
#include "UIElements.h"
#include "Score.h"

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

    void drawMainMenu();
    void drawGameOver();
    void drawPlayingControls();

private:
    android_app *application;
    AAssetManager *assetManager;

    Renderer renderer;
    World world;

    // упрощенный менеджер мешей
    std::unordered_map<EntityId, std::shared_ptr<MeshComponent>> meshStorage;
    std::unordered_map<EntityId, AnimationComponent> textureStorage;
    std::vector<std::unique_ptr<ISystem>> systems;

    GameState gameState = GameState::MainMenu;

    std::vector<Button> mainMenuButtons;
    std::vector<Button> gameoverButtons;
    Button attackControl;

    ScoreInfo scoreInfo;
};

#endif //PUPPY_BIRD_GAME_H
