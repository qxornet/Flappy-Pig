//
// Created by ilya on 26.06.2026.
//

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "Game.h"
#include "AndroidOut.h"
#include "TextureAsset.h"

Game::Game(android_app *app) : application(app), renderer(app), assetManager(app->activity->assetManager)
{
    loadMeshes();
    loadTextures();
    loadSystems();
    createWorld();
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
        aout << "Pointer(s): ";

        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);

        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                if (y < (float)screenHeight * 0.5f)
                    world.player.inputY = 1;
                else
                    world.player.inputY = -1;
                break;

            case AMOTION_EVENT_ACTION_CANCEL:
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                world.player.inputY = 0;
                break;
            default:
                aout << "Unknown MotionEvent Action: " << action;
        }
        aout << std::endl;
    }

    // Очищаем список событий движения,
    // чтобы главный поток мог повторно использовать буфер.
    android_app_clear_motion_events(inputBuffer);

    // Обрабатываем события клавиатуры.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
        aout << "Key: " << keyEvent.keyCode <<" ";
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
                aout << "Key Down";
                break;
            case AKEY_EVENT_ACTION_UP:
                aout << "Key Up";
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                // Устарело начиная с Android API 29.
                aout << "Multiple Key Actions";
                break;
            default:
                aout << "Unknown KeyEvent Action: " << keyEvent.action;
        }
        aout << std::endl;
    }
    // Также очищаем список событий клавиатуры.
    android_app_clear_key_events(inputBuffer);
}

void Game::update(float dt)
{
    world.borderX = renderer.getProjectionHalfHeight() * renderer.getRatio();
    world.borderY = renderer.getProjectionHalfHeight();

    for(auto& system : systems)
        system->update(dt, world);
}

void Game::render()
{
    renderer.beginFrame();

    renderer.draw(world.background.mesh, world.background.transform, world.background.material);
    // TODO: добавить рендер grounds и buildings
    renderer.draw(world.player.mesh, world.player.transform, world.player.material);

    for(auto &bird : world.birds) {
        if(!bird.isShow) continue;
        renderer.draw(bird.mesh, bird.transform, bird.material);
    }
    renderer.endFrame();
}

void Game::loadMeshes()
{
    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{0, 0}), // 0
            Vertex(Vector3{-0.5, 0.5, 1}, Vector2{1, 0}), // 1
            Vertex(Vector3{-0.5, -0.5, 1}, Vector2{1, 1}), // 2
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{0, 1}) // 3
    };

    std::vector<Vertex> playerVertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{1, 0}), // 0
            Vertex(Vector3{-0.5, 0.5, 1}, Vector2{0, 0}), // 1
            Vertex(Vector3{-0.5, -0.5, 1}, Vector2{0, 1}), // 2
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{1, 1}) // 3
    };

    std::vector<Vertex> backgroundVertices = {
            Vertex(Vector3{1, 1, 0}, Vector2{1, 0}), // 0
            Vertex(Vector3{-3, 1, 0}, Vector2{0, 0}), // 1
            Vertex(Vector3{-3, -1, 0}, Vector2{0, 1}), // 2
            Vertex(Vector3{1, -1, 0}, Vector2{1, 1}) // 3
    };

    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    meshStorage[EntityId::eBackground] = std::make_shared<MeshComponent>(backgroundVertices, indices);
    meshStorage[EntityId::ePlayer] = std::make_shared<MeshComponent>(playerVertices, indices);
    meshStorage[EntityId::eBird] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eFighter] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eMeteor] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomb] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBullets] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eTerrain] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBuilding] = std::make_shared<MeshComponent>(vertices, indices);
}

void Game::loadTextures()
{
    AnimationComponent backgroundTextures;
    backgroundTextures.clips[AnimationType::Idle] = {TextureAsset::loadAsset(assetManager, "backgrounds/background01.png")};
    textureStorage[EntityId::eBackground] = backgroundTextures;

    AnimationComponent playerTextures;
    playerTextures.frameTime = 100; // сменять текстуру раз в 100мс
    playerTextures.current = AnimationType::Run;
    playerTextures.frame = 0;
    playerTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "player/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "player/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "player/run_anim03.png"),
    };
    textureStorage[EntityId::ePlayer] = playerTextures;

    AnimationComponent birdTextures;
    birdTextures.frameTime = 150;
    birdTextures.current = AnimationType::Run;
    birdTextures.frame = 0;
    birdTextures.clips[AnimationType::Run] = {
            TextureAsset::loadAsset(assetManager, "bird/run_anim01.png"),
            TextureAsset::loadAsset(assetManager, "bird/run_anim02.png"),
            TextureAsset::loadAsset(assetManager, "bird/run_anim03.png"),
    };
    birdTextures.clips[AnimationType::Danger] = {
            TextureAsset::loadAsset(assetManager, "bird/danger_anim01.png"),
            TextureAsset::loadAsset(assetManager, "bird/danger_anim02.png"),
            TextureAsset::loadAsset(assetManager, "bird/danger_anim03.png"),
    };
    birdTextures.clips[AnimationType::Die] = {
            TextureAsset::loadAsset(assetManager, "bird/death_anim01.png"),
    };
    textureStorage[EntityId::eBird] = birdTextures;
}

void Game::createWorld()
{
    // загурзка бекграунда
    world.background.mesh = meshStorage[EntityId::eBackground];
    world.background.transform.scale = {2.5f, 2.5f, 2.5f};
    world.background.transform.position.x = 2.5f;
    world.background.transform.position.y = 0.5f;
    world.background.material = textureStorage[EntityId::eBackground].clips[AnimationType::Idle][0];

    // загрузка игрока
    world.player.mesh = meshStorage[EntityId::ePlayer];
    world.player.material.texture = textureStorage[EntityId::ePlayer].clips[AnimationType::Run][0];
    world.player.animation = textureStorage[EntityId::ePlayer];
    world.player.transform.scale = {.45f, .45f, .45f};
    world.player.transform.position.x = -0.5f;
    world.player.health.points = 100;
    world.player.health.max_points = 100;
    world.player.movement.maxSpeed = 2.5f;
    world.player.movement.velocity.y = 1.0f;

    std::srand(std::time(nullptr));
    for(int i = 0; i < 50; i++) {
        Bird bird;
        bird.mesh = meshStorage[EntityId::eBird];
        bird.material.texture = textureStorage[EntityId::eBird].clips[AnimationType::Run][0 + (rand() % 2)];
        bird.animation = textureStorage[EntityId::eBird];
        bird.transform.scale = {0.15f, 0.15f, 0.15f};
        bird.transform.position.z = 2.0f;
        bird.health.points = 1;
        bird.health.max_points = 1;
        bird.movement.velocity.x = 0.3f;
        bird.movement.maxSpeed = 1.5f;
        bird.isShow = true;
        if(i >= 2) bird.isShow = false;
        world.birds.push_back(bird);
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
}
