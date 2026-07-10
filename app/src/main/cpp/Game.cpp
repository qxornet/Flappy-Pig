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
//        aout << "Pointer(s): ";

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
//                aout << "Unknown MotionEvent Action: " << action;
        }
//        aout << std::endl;
    }

    // Очищаем список событий движения,
    // чтобы главный поток мог повторно использовать буфер.
    android_app_clear_motion_events(inputBuffer);

    // Обрабатываем события клавиатуры.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
//        aout << "Key: " << keyEvent.keyCode <<" ";
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
//                aout << "Key Down";
                break;
            case AKEY_EVENT_ACTION_UP:
//                aout << "Key Up";
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                // Устарело начиная с Android API 29.
//                aout << "Multiple Key Actions";
                break;
            default:
//                aout << "Unknown KeyEvent Action: " << keyEvent.action;
        }
//        aout << std::endl;
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

    for(auto &bomber : world.bombers) {
        if(!bomber.isShow) continue;
        renderer.draw(bomber.mesh, bomber.transform, bomber.material);
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


    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    meshStorage[EntityId::eBackground] = std::make_shared<MeshComponent>(backgroundVertices, indices);
    meshStorage[EntityId::ePlayer] = std::make_shared<MeshComponent>(playerVertices, indices);
    meshStorage[EntityId::eBird] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eFighter] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eMeteor] = std::make_shared<MeshComponent>(vertices, indices);
    meshStorage[EntityId::eBomber] = std::make_shared<MeshComponent>(bomberVertices, indices);
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
            TextureAsset::loadAsset(assetManager, "bomber/run_without_bomb_anim01.png"),
            TextureAsset::loadAsset(assetManager, "bomber/run_without_bomb_anim02.png")
    };
    bomberTextures.clips[AnimationType::Danger] = {
            TextureAsset::loadAsset(assetManager, "bomber/danger_without_bomb_anim01.png")
    };
    textureStorage[EntityId::eBomber] = bomberTextures;
}

void Game::createWorld()
{
    EntityFactory factory(meshStorage, textureStorage);

    world.background = factory.createBackground();
    world.player = factory.createPlayer();

    world.birds.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        auto bird = factory.createBird();
        bird.isShow = (i < 2);
        world.birds.push_back(std::move(bird));
    }

    world.bombers.reserve(50);
    for (int i = 0; i < 50; ++i)
    {
        auto bomber = factory.createBomber();
        bomber.isShow = (i < 1);
        world.bombers.push_back(std::move(bomber));
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
