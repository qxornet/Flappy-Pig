#include <jni.h>

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <game-activity/GameActivity.h>

#include "AndroidOut.h"
#include "Game.h"

extern "C" {

/*!
 * Обрабатывает команды, отправленные этому Android-приложению.
 * @param pApp приложение, от которого приходят команды
 * @param cmd команда для обработки
 */
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            pApp->userData = new Game(pApp);
            break;

        case APP_CMD_TERM_WINDOW:
            if (pApp->userData) {
                auto *game = reinterpret_cast<Game *>(pApp->userData);
                pApp->userData = nullptr;
                delete game;
            }
            break;

        default:
            break;
    }
}

/*!
 * Включает обработку нужных событий движения.
 * Необработанные события передаются ОС для дальнейшей обработки.
 * В данном примере разрешены только устройства указателя и джойстики.
 *
 * @param motionEvent только что полученное событие GameActivityMotionEvent.
 * @return true, если событие пришло от указателя или джойстика,
 *         false для всех остальных устройств ввода.
 */
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

/*!
 * Главная точка входа для Native Activity.
 */
void android_main(struct android_app *pApp) {
    // Можно удалить. Полезно для проверки того, что код действительно запущен.
    aout << "Welcome to android_main" << std::endl;

    // Регистрируем обработчик событий Android.
    pApp->onAppCmd = handle_cmd;

    // Устанавливаем фильтр событий ввода.
    // Передайте NULL, если приложение хочет обрабатывать все события.
    // Для клавиатурного ввода используется стандартный
    // default_key_filter() из android_native_app_glue.c.
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    using Clock = std::chrono::steady_clock;
    auto lastTime = Clock::now();

    // Типичный игровой цикл обработки событий.
    // Выполняется до тех пор, пока приложение не будет уничтожено.
    do {
        // Обрабатываем все ожидающие события перед выполнением игровой логики.
        bool done = false;
        while (!done) {
            // 0 означает неблокирующий режим.
            int timeout = 0;
            int events;
            android_poll_source *pSource;

            int result = ALooper_pollOnce(
                    timeout,
                    nullptr,
                    &events,
                    reinterpret_cast<void **>(&pSource)
            );

            switch (result) {
                case ALOOPER_POLL_TIMEOUT:
                    [[clang::fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // До истечения тайм-аута не произошло событий
                    // либо цикл был явно пробуждён.
                    // Завершаем проверку событий.
                    done = true;
                    break;

                case ALOOPER_EVENT_ERROR:
                    aout << "ALooper_pollOnce returned an error" << std::endl;
                    break;

                case ALOOPER_POLL_CALLBACK:
                    break;

                default:
                    if (pSource) {
                        pSource->process(pApp, pSource);
                    }
            }
        }

        // Проверяем, связано ли с приложением пользовательское состояние.
        // Оно назначается в handle_cmd.
        if (pApp->userData) {
            // Мы знаем, что userData содержит Renderer,
            // поэтому приводим тип через reinterpret_cast.
            // Если измените тип userData, не забудьте исправить это место.
            auto *game = reinterpret_cast<Game *>(pApp->userData);

            auto currentTime = Clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            // Обработка пользовательского ввода.
            game->handleInput();
            game->update(deltaTime);
            game->render();
        }

    } while (!pApp->destroyRequested);
}

}