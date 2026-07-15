#include "UIElements.h"
#include "AndroidOut.h"

std::vector<Button> initMainMenu(AAssetManager *assetManager, GameState *gameState)
{
    std::vector<Button> mainMenuButtons;

    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.5, 0.12, 10}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, 0.12, 10}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, -0.12, 10}, Vector2{0, 1}),
            Vertex(Vector3{0.5, -0.12, 10}, Vector2{1, 1})
    };

    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    Button startButton;
    Button aboutButton;
    Button exitButton;

    startButton.mesh = std::make_shared<MeshComponent>(vertices, indices);
    aboutButton.mesh = std::make_shared<MeshComponent>(vertices, indices);
    exitButton.mesh = std::make_shared<MeshComponent>(vertices, indices);

    startButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/start_button.png");
    aboutButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/about_button.png");
    exitButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/exit_button.png");

    startButton.transform.position.y = -0.35f;
    aboutButton.transform.position.y = -0.7f;
    exitButton.transform.position.y = -1.05f;

    startButton.onClick = [gameState]() {
        if(!gameState) return;
        *gameState = GameState::Playing;
    };

    exitButton.onClick = [gameState]() {
        if(!gameState) return;
        *gameState = GameState::Exit;
    };

    mainMenuButtons.push_back(startButton);
    mainMenuButtons.push_back(aboutButton);
    mainMenuButtons.push_back(exitButton);

    return mainMenuButtons;
}

std::vector<Button> initGameoverMenu(AAssetManager *assetManager, GameState *gameState)
{
    std::vector<Button> gameoverMenuButtons;

    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.5, 0.12, 10}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, 0.12, 10}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, -0.12, 10}, Vector2{0, 1}),
            Vertex(Vector3{0.5, -0.12, 10}, Vector2{1, 1})
    };

    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    Button restartButton;
    Button exitButton;

    restartButton.onClick = [gameState]() {
        if(!gameState) return;
        *gameState = GameState::Restart;
    };

    exitButton.onClick = [gameState]() {
        if(!gameState) return;
        *gameState = GameState::Exit;
    };

    restartButton.mesh = std::make_shared<MeshComponent>(vertices, indices);
    exitButton.mesh = std::make_shared<MeshComponent>(vertices, indices);

    restartButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/restart_button.png");
    exitButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/exit_button.png");

    restartButton.transform.position.y = -0.7f;
    exitButton.transform.position.y = -1.05f;

    gameoverMenuButtons.push_back(restartButton);
    gameoverMenuButtons.push_back(exitButton);

    return gameoverMenuButtons;
}

Button initAttackControl(AAssetManager *assetManager, bool *enableAttack)
{
    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.2, 0.2, 10}, Vector2{1, 0}),
            Vertex(Vector3{-0.2, 0.2, 10}, Vector2{0, 0}),
            Vertex(Vector3{-0.2, -0.2, 10}, Vector2{0, 1}),
            Vertex(Vector3{0.2, -0.2, 10}, Vector2{1, 1})
    };

    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    Button attackButton;

    attackButton.mesh = std::make_shared<MeshComponent>(vertices, indices);
    attackButton.material.texture = TextureAsset::loadAsset(assetManager, "ui/attack_button.png");
    attackButton.transform.position.y = -1.5f;
    attackButton.transform.position.x = .5f;

    attackButton.onClick = [enableAttack]() {
        if(!enableAttack) return;
        *enableAttack = true;
    };

    return attackButton;
}