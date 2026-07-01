//
// Created by ilya on 26.06.2026.
//

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "Game.h"
#include "AndroidOut.h"
#include "TextureAsset.h"

Game::Game(android_app *app) : app_(app), renderer(app), assetManager(app->activity->assetManager)
{
    loadMeshes();
}

void Game::handleInput()
{
    // Обрабатываем все накопленные события ввода.
    auto *inputBuffer = android_app_swap_input_buffers(app_);
    if (!inputBuffer) {
        // Событий пока нет.
        return;
    }

    // Обрабатываем события движения.
    // motionEventsCount может быть равен 0.
    for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
        auto &motionEvent = inputBuffer->motionEvents[i];
        auto action = motionEvent.action;

        // Получаем индекс указателя,
        // применяя маску и сдвиг битов.
        auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        aout << "Pointer(s): ";

        // Получаем координаты X и Y,
        // если это не ACTION_MOVE.
        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);

        // Определяем тип действия и обрабатываем событие.
        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                models[1].moveY(-y);
                aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                     << "Pointer Down";
                break;

            case AMOTION_EVENT_ACTION_CANCEL:
                // Обрабатываем CANCEL как событие UP:
                // приложение ничего не делает,
                // кроме удаления указателя из локального кэша,
                // если такой используется.
                //
                // Переход к следующему case выполнен намеренно.
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                models[1].moveY(y);
                aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                     << "Pointer Up";
                break;

            case AMOTION_EVENT_ACTION_MOVE:
                // Для ACTION_MOVE отсутствует индекс указателя.
                // Доступен только снимок состояния всех активных указателей.
                // Поэтому приложению необходимо хранить предыдущее состояние,
                // чтобы определить, какие именно указатели переместились.
                for (auto index = 0; index < motionEvent.pointerCount; index++) {
                    pointer = motionEvent.pointers[index];
                    x = GameActivityPointerAxes_getX(&pointer);
                    y = GameActivityPointerAxes_getY(&pointer);
                    aout << "(" << pointer.id << ", " << x << ", " << y << ")";

                    if (index != (motionEvent.pointerCount - 1)) aout << ",";
                    aout << " ";
                }
                aout << "Pointer Move";
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

    std::srand(std::time(0));

    auto& obj1 = models[0];
    auto& obj2 = models[1];

    obj1.moveX(0.001f);
    if((obj1.getPosition().x-0.5) >= 1.0f) {
        obj1.setY(0.5f + (float)(rand() % 500)/1000.0f); // диапазон 0.5f до 1.0f
        obj1.setX(-1.5f);
    }
}

void Game::render()
{
    renderer.beginFrame();
    for(auto &model : models)
        renderer.draw(model.getMesh(), model.getTransform(), model.getMaterial());
    renderer.endFrame();
}

void Game::loadMeshes()
{
    /*
     * Это квадрат:
     *
     *  0 --- 1
     *  |  \  |
     *  3 --- 2
     */
    std::vector<Vertex> vertices = {
            Vertex(Vector3{0.5, 0.5, 1}, Vector2{0, 0}), // 0
            Vertex(Vector3{-0.5, 0.5, 1}, Vector2{1, 0}), // 1
            Vertex(Vector3{-0.5, -0.5, 1}, Vector2{1, 1}), // 2
            Vertex(Vector3{0.5, -0.5, 1}, Vector2{0, 1}) // 3
    };
    std::vector<Index> indices = {
            0, 1, 2, 0, 2, 3
    };

    // Загружаем изображение и назначаем его квадрату.
    auto spAndroidRobotTexture = TextureAsset::loadAsset(assetManager, "android_robot.png");
    auto spAndroidRobot2Texture = TextureAsset::loadAsset(assetManager, "android_robot.png");

    meshStorage[EntityId::ePlayer] = std::make_shared<MeshComponent>(vertices, indices);

    // Создаём модель и добавляем её в конец списка рендеринга.
    models.emplace_back(meshStorage[EntityId::ePlayer], spAndroidRobotTexture);
    models.emplace_back(meshStorage[EntityId::ePlayer], spAndroidRobot2Texture);
}

