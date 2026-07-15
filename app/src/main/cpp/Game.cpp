//
// Created by ilya on 26.06.2026.
//

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "Game.h"
#include "AndroidOut.h"
#include "TextureAsset.h"
#include "EntityFactory.h"

Game::Game(android_app *app) : application(app), renderer(app), assetManager(app->activity->assetManager)
{
    std::srand(std::time(nullptr));

    loadMeshes();
    loadTextures();
    loadSystems();
    createWorld();

    mainMenuButtons = initMainMenu(assetManager, &gameState);
    gameoverButtons = initGameoverMenu(assetManager, &gameState);
    attackControl = initAttackControl(assetManager, &world.player.attack.shooting);
}

bool isInside(Button& b, float x, float y)
{
    return
            x >= b.transform.position.x - b.mesh->width * 0.5f &&
            x <= b.transform.position.x + b.mesh->width * 0.5f &&
            y >= b.transform.position.y - b.mesh->height * 0.5f &&
            y <= b.transform.position.y + b.mesh->height * 0.5f;
}

void processButtons(float x, float y, std::vector<Button>& buttons)
{
    for (auto& button : buttons)
    {
        if (isInside(button, x, y))
        {
            if (button.onClick)
                button.onClick();

            break;
        }
    }
}

bool processButton(float x, float y, Button &button)
{
    if (isInside(button, x, y))
    {
        if (button.onClick)
            button.onClick();

        return true;
    }

    return false;
}

void Game::handleInput()
{
    // Обрабатываем все накопленные события ввода.
    auto *inputBuffer = android_app_swap_input_buffers(application);
    if (!inputBuffer) {
        // Событий пока нет.
        return;
    }

    int screenHeight = renderer.getHeight();
    int screenWidth = renderer.getWidth();

    for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
        auto &motionEvent = inputBuffer->motionEvents[i];
        auto action = motionEvent.action;

        auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);

        float worldX = (x / screenWidth) * world.borderX * 2.0f - world.borderX;
        float worldY = world.borderY - (y / screenHeight) * world.borderY * 2.0f;

        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                switch (gameState)
                {
                    case GameState::MainMenu:
                        processButtons(worldX, worldY, mainMenuButtons);
                        break;

                    case GameState::GameOver:
                        processButtons(worldX, worldY, gameoverButtons);
                        break;

                    case GameState::Playing:
                        if(processButton(worldX, worldY, attackControl)) break;

                        if (worldY > 0)
                            world.player.controller.targetDirection = 1;
                        else
                            world.player.controller.targetDirection = -1;
                        break;
                }
                break;

            case AMOTION_EVENT_ACTION_CANCEL:
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                world.player.controller.targetDirection = 0;
                world.player.attack.shooting = false;
                break;
            default:
        }
    }

    // Очищаем список событий движения,
    // чтобы главный поток мог повторно использовать буфер.
    android_app_clear_motion_events(inputBuffer);

    // Обрабатываем события клавиатуры.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
                break;
            case AKEY_EVENT_ACTION_UP:
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                break;
            default:
        }
    }
    // Также очищаем список событий клавиатуры.
    android_app_clear_key_events(inputBuffer);
}

void Game::update(float dt)
{
    world.borderX = renderer.getProjectionHalfHeight() * renderer.getRatio();
    world.borderY = renderer.getProjectionHalfHeight();

    for(auto& system : systems)
        system->update(dt, world, gameState);

    switch(gameState) {
        case GameState::Exit:
            GameActivity_finish(application->activity);
            break;
        case GameState::Restart:
            createWorld();
            gameState = GameState::Playing;
        default:
    }
}

void Game::render()
{
    renderer.beginFrame();

    renderer.draw(world.background.mesh, world.background.transform, world.background.material);
    renderer.draw(world.secondBackground.mesh, world.secondBackground.transform, world.secondBackground.material);
    // TODO: добавить рендер grounds и buildings

    for(auto &bullet : world.playerBullets) {
        if(!bullet.isShow) continue;
        renderer.draw(bullet.mesh, bullet.transform, bullet.material);
    }

    renderer.draw(world.player.mesh, world.player.transform, world.player.material);

    for(auto &bird : world.birds) {
        if(!bird.isShow) continue;
        renderer.draw(bird.mesh, bird.transform, bird.material);
    }

    for(auto &bomber : world.bombers) {
        if(!bomber.isShow) continue;
        renderer.draw(bomber.mesh, bomber.transform, bomber.material);
    }

    for(auto &fighter : world.fighters) {
        if(!fighter.isShow) continue;
        renderer.draw(fighter.mesh, fighter.transform, fighter.material);
    }

    for(auto &bullet : world.enemyBullets) {
        if(!bullet.isShow) continue;
        renderer.draw(bullet.mesh, bullet.transform, bullet.material);
    }

    switch (gameState)
    {
        case GameState::MainMenu:
            drawMainMenu();
            break;

        case GameState::GameOver:
            drawGameOver();
            break;

        case GameState::Exit:
            break;

        default:
            drawPlayingControls();
            break;
    }

    renderer.endFrame();
}

void Game::loadMeshes()
{
    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, 0.5, 1}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, -0.5, 1}, Vector2{1, 1}),
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{0, 1})
    };

    std::vector<Vertex> playerVertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, 0.5, 1}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, -0.5, 1}, Vector2{0, 1}),
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{1, 1})
    };

    std::vector<Vertex> playerBulletVertices = {
            Vertex(Vector3{0.5, 0.1, 1}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, 0.1, 1}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, -0.1, 1}, Vector2{0, 1}),
            Vertex(Vector3{0.5, -0.1, 1}, Vector2{1, 1})
    };


    std::vector<Vertex> backgroundVertices = {
            Vertex(Vector3{1, 1, 0}, Vector2{1, 0}),
            Vertex(Vector3{-3, 1, 0}, Vector2{0, 0}),
            Vertex(Vector3{-3, -1, 0}, Vector2{0, 1}),
            Vertex(Vector3{1, -1, 0}, Vector2{1, 1})
    };

    std::vector<Vertex> bomberVertices = {
            Vertex(Vector3{1, 1, 0}, Vector2{1, 0}),
            Vertex(Vector3{-2.5, 1, 0}, Vector2{0, 0}),
            Vertex(Vector3{-2.5, -1, 0}, Vector2{0, 1}),
            Vertex(Vector3{1, -1, 0}, Vector2{1, 1})
    };

    std::vector<Vertex> fighterVertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{0, 0}),
            Vertex(Vector3{-1.0, 0.5, 1}, Vector2{1, 0}),
            Vertex(Vector3{-1.0, -0.5, 1}, Vector2{1, 1}),
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{0, 1})
    };

    std::vector<Vertex> enemyBulletVertices = {
            Vertex(Vector3{0.5, 0.1, 1}, Vector2{0, 0}),
            Vertex(Vector3{-0.5, 0.1, 1}, Vector2{1, 0}),
            Vertex(Vector3{-0.5, -0.1, 1}, Vector2{1, 1}),
            Vertex(Vector3{0.5, -0.1, 1}, Vector2{0, 1})
    };

    std::vector<Vertex> digitVertices = {
            Vertex(Vector3{0.4, 0.5, 1}, Vector2{1, 0}),
            Vertex(Vector3{-0.4, 0.5, 1}, Vector2{0, 0}),
            Vertex(Vector3{-0.4, -0.5, 1}, Vector2{0, 1}),
            Vertex(Vector3{0.4, -0.5, 1}, Vector2{1, 1})
    };

    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    meshStorage[EntityId::eBackground] = std::make_shared<MeshComponent>(backgroundVertices, indices);
    meshStorage[EntityId::ePlayer] = std::make_shared<MeshComponent>(playerVertices, indices);
    meshStorage[EntityId::eBird] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eFighter] = std::make_shared<MeshComponent>(fighterVertices, indices);
    meshStorage[EntityId::eMeteor] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(bomberVertices, indices);
    meshStorage[EntityId::eBomb] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eEnemyBullet] = std::make_shared<MeshComponent>(enemyBulletVertices, indices);
    meshStorage[EntityId::ePlayerBullet] = std::make_shared<MeshComponent>(playerBulletVertices, indices);
    meshStorage[EntityId::eTerrain] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBuilding] = std::make_shared<MeshComponent>(vertices, indices);

    scoreInfo.digitMesh = std::make_shared<MeshComponent>(digitVertices, indices);
    for(auto &digit : scoreInfo.digits) {
        digit.mesh = scoreInfo.digitMesh;
    }
}

void Game::loadTextures()
{
    AnimationComponent backgroundTextures;
    backgroundTextures.clips[AnimationType::Idle] = {TextureAsset::loadAsset(assetManager, "backgrounds/background01.png")};
    textureStorage[EntityId::eBackground] = backgroundTextures;

    AnimationComponent playerTextures;
    playerTextures.current = AnimationType::Run;
    playerTextures.frame = 0;
    playerTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "player/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "player/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "player/run_anim03.png"),
    };
    textureStorage[EntityId::ePlayer] = playerTextures;

    AnimationComponent birdTextures;
    birdTextures.current = AnimationType::Run;
    birdTextures.frame = 0;
    birdTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "bird/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "bird/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "bird/run_anim03.png"),
    };
    birdTextures.clips[AnimationType::DangerFar] = {
            TextureAsset::loadAsset(assetManager, "bird/danger_anim01.png"),
    };
    birdTextures.clips[AnimationType::DangerMid] = {
            TextureAsset::loadAsset(assetManager, "bird/danger_anim02.png"),
    };
    birdTextures.clips[AnimationType::DangerNear] = {
            TextureAsset::loadAsset(assetManager, "bird/danger_anim03.png"),
    };
    birdTextures.clips[AnimationType::Die] = {
            TextureAsset::loadAsset(assetManager, "bird/death_anim01.png"),
    };
    textureStorage[EntityId::eBird] = birdTextures;

    AnimationComponent bomberTextures;
    bomberTextures.current = AnimationType::Run;
    bomberTextures.frame = 0;
    bomberTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "bomber/run_with_bomb_anim01.png"),
            TextureAsset::loadAsset(assetManager, "bomber/run_with_bomb_anim02.png"),
            TextureAsset::loadAsset(assetManager, "bomber/run_with_bomb_anim03.png")
    };
    bomberTextures.clips[AnimationType::Danger] = {
            TextureAsset::loadAsset(assetManager, "bomber/danger_with_bomb_anim01.png")
    };
    textureStorage[EntityId::eBomber] = bomberTextures;

    AnimationComponent fighterTextures;
    fighterTextures.current = AnimationType::Run;
    fighterTextures.frame = 0;
    fighterTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "fighter/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "fighter/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "fighter/run_anim03.png"),
    };
    textureStorage[EntityId::eFighter] = fighterTextures;

    AnimationComponent enemyBulletTextures;
    enemyBulletTextures.current = AnimationType::Run;
    enemyBulletTextures.frame = 0;
    enemyBulletTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "fighter/bullets/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "fighter/bullets/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "fighter/bullets/run_anim03.png"),
    };
    textureStorage[EntityId::eEnemyBullet] = enemyBulletTextures;

    AnimationComponent playerBulletTextures;
    playerBulletTextures.current = AnimationType::Run;
    playerBulletTextures.frame = 0;
    playerBulletTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "player/bullets/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "player/bullets/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "player/bullets/run_anim03.png"),
    };
    textureStorage[EntityId::ePlayerBullet] = playerBulletTextures;

    scoreInfo.textures.resize(10);
    for (int i = 0; i < 10; ++i) {
        scoreInfo.textures[i] = TextureAsset::loadAsset(assetManager,"digits/" + std::to_string(i) + ".png");
    }

}

void Game::createWorld()
{
    EntityFactory factory(meshStorage, textureStorage);

    world.background = factory.createBackground();
    world.secondBackground = factory.createBackground();
    world.secondBackground.transform.position.x = (world.background.mesh->width * world.background.transform.scale.x)
                                                            + world.background.transform.position.x;
    world.secondBackground.transform_default = world.secondBackground.transform;
    world.player = factory.createPlayer();

    world.birds.clear();
    world.birds.reserve(30);
    for (int i = 0; i < 30; ++i) {
        auto bird = factory.createBird();
        world.birds.push_back(std::move(bird));
    }

    world.bombers.clear();
    world.bombers.reserve(10);
    for (int i = 0; i < 10; ++i) {
        auto bomber = factory.createBomber();
        world.bombers.push_back(std::move(bomber));
    }

    world.fighters.clear();
    world.fighters.reserve(20);
    for (int i = 0; i < 20; ++i) {
        auto fighter = factory.createFighter();
        world.fighters.push_back(std::move(fighter));
    }

    world.enemyBullets.clear();
    world.enemyBullets.reserve(120);
    for(int i = 0; i < 120; ++i) {
        auto bullet = factory.createEnemyBullet();
        world.enemyBullets.push_back(std::move(bullet));
    }

    world.playerBullets.clear();
    world.playerBullets.reserve(100);
    for(int i = 0; i < 100; ++i) {
        auto bullet = factory.createPlayerBullet();
        world.playerBullets.push_back(std::move(bullet));
    }
}

void Game::loadSystems()
{
    systems.push_back(std::make_unique<MovementSystem>());
    systems.push_back(std::make_unique<CollisionSystem>());
    systems.push_back(std::make_unique<AnimationSystem>());
    systems.push_back(std::make_unique<InputSystem>());
    systems.push_back(std::make_unique<SpawnSystem>());
    systems.push_back(std::make_unique<UltimateSystem>());
    systems.push_back(std::make_unique<EventSystem>());
    systems.push_back(std::make_unique<HealthSystem>());
}

void Game::drawMainMenu()
{
    float startYOffset = -0.35f;
    for(auto &button : mainMenuButtons) {
        button.transform.position.y = startYOffset;
        renderer.draw(button.mesh, button.transform, button.material);
        startYOffset += -0.35f;
    }
}

void Game::drawGameOver()
{
    float startYOffset = -0.35f;
    for(auto &button : gameoverButtons) {
        button.transform.position.y = startYOffset;
        renderer.draw(button.mesh, button.transform, button.material);
        startYOffset += -0.35f;
    }
}

void Game::drawPlayingControls()
{
    attackControl.transform.position.x = world.borderX - 1.5f;
    renderer.draw(attackControl.mesh, attackControl.transform, attackControl.material);
    scoreInfo.draw(renderer, world.player.score.count);
}