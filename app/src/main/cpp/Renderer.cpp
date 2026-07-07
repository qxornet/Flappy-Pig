#include "Renderer.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <chrono>
#include <android/imagedecoder.h>

#include "AndroidOut.h"
#include "Shader.h"
#include "Utility.h"

//! Выполняет glGetString и выводит результат в logcat
#define PRINT_GL_STRING(s) {aout << #s": "<< glGetString(s) << std::endl;}

/*!
 * @brief Если glGetString возвращает список элементов, разделённых пробелами,
 * выводит каждый элемент с новой строки.
 *
 * Работает следующим образом: создаётся istringstream из входной C-строки.
 * Затем на его основе формируется вектор, где каждый элемент соответствует
 * отдельному элементу исходной строки. После этого цикл foreach проходит
 * по вектору и выводит содержимое в logcat через @a aout.
 */

#define PRINT_GL_STRING_AS_LIST(s) { \
std::istringstream extensionStream((const char *) glGetString(s));\
std::vector<std::string> extensionList(\
        std::istream_iterator<std::string>{extensionStream},\
        std::istream_iterator<std::string>());\
aout << #s":\n";\
for (auto& extension: extensionList) {\
    aout << extension << "\n";\
}\
aout << std::endl;\
}

//! Цвет Cornflower Blue. Может напрямую передаваться в glClearColor
#define CORNFLOWER_BLUE 100 / 255.f, 149 / 255.f, 237 / 255.f, 1

// Вершинный шейдер. Обычно его загружают из ресурсов (assets)
static const char *vertex = R"vertex(#version 300 es
in vec3 inPosition;
in vec2 inUV;

out vec2 fragUV;

uniform mat4 uProjection;

uniform vec3 objPosition;
uniform vec3 objScale;

void main() {
    fragUV = inUV;
    gl_Position = uProjection * vec4(inPosition * objScale + objPosition, 1.0);
}
)vertex";

// Фрагментный шейдер. Обычно его загружают из ресурсов (assets)
static const char *fragment = R"fragment(#version 300 es
precision mediump float;

in vec2 fragUV;

uniform sampler2D uTexture;

out vec4 outColor;

void main() {
    outColor = texture(uTexture, fragUV);
}
)fragment";

/*!
 * Половина высоты проекционной матрицы.
 * Создаёт область рендеринга высотой 4 единицы
 * в диапазоне от -2 до 2.
 */
static constexpr float kProjectionHalfHeight = 2.f;

/*!
 * Расстояние до ближней плоскости проекции.
 * Поскольку используется ортографическая проекция,
 * удобно использовать отрицательные значения для
 * сортировки объектов и предотвращения z-fighting возле нуля.
 */
static constexpr float kProjectionNearPlane = -10.f;

/*!
 * Расстояние до дальней плоскости проекции.
 * Поскольку используется ортографическая проекция,
 * удобно расположить дальнюю плоскость симметрично
 * относительно нуля относительно ближней.
 */
static constexpr float kProjectionFarPlane = 10.f;

Renderer::~Renderer() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
            context_ = EGL_NO_CONTEXT;
        }
        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
            surface_ = EGL_NO_SURFACE;
        }
        eglTerminate(display_);
        display_ = EGL_NO_DISPLAY;
    }
}

void Renderer::beginFrame()
{
    // Проверяем, не изменился ли размер поверхности.
    // В иммерсивном режиме это необходимо делать каждый кадр,
    // так как других уведомлений об изменении области рендеринга не будет.
    updateRenderArea();

    // При изменении области рендеринга необходимо пересоздать
    // проекционную матрицу. Это справедливо даже при использовании
    // другой матрицы проекции, так как соотношение сторон могло измениться.
    if (shaderNeedsNewProjectionMatrix_) {
        // Временная проекционная матрица, размещённая в стеке.
        // Используется порядок хранения column-major.
        float projectionMatrix[16] = {0};

        // Строим ортографическую матрицу проекции для 2D-рендеринга.
        Utility::buildOrthographicMatrix(
                projectionMatrix,
                kProjectionHalfHeight,
                getRatio(),
                kProjectionNearPlane,
                kProjectionFarPlane);

        // Передаём матрицу в шейдер.
        // Важно: шейдер должен быть активирован.
        // Так как в демо используется только один шейдер,
        // предполагаем, что он уже активен.
        shader_->setProjectionMatrix(projectionMatrix);

        // Чтобы не пересоздавать матрицу каждый кадр.
        shaderNeedsNewProjectionMatrix_ = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void Renderer::draw(std::shared_ptr<MeshComponent> mesh, const TransformComponent& transform, const MaterialComponent& material) {
    shader_->draw(mesh, transform, material);
}

void Renderer::endFrame()
{
    auto swapResult = eglSwapBuffers(display_, surface_);
    assert(swapResult == EGL_TRUE);
}

void Renderer::initRenderer() {
    // Выбираем параметры рендеринга.
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    // Получаем дисплей по умолчанию.
    // Обычно это именно то, что нужно на Android.
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // Выясняем количество доступных конфигураций EGL.
    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    // Получаем список конфигураций.
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    // Ищем подходящую конфигурацию.
    // Если дополнительные требования не важны,
    // можно просто взять первую найденную.
    // Иначе здесь можно реализовать собственную эвристику.
    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    aout << "Found config with " << red << ", " << green << ", " << blue << ", "
                         << depth << std::endl;
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

    aout << "Found " << numConfigs << " configs" << std::endl;
    aout << "Chose " << config << std::endl;

    // Создаём поверхность окна.
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    EGLSurface surface = eglCreateWindowSurface(display, config, app_->window, nullptr);

    // Создаём контекст OpenGL ES 3.
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

    // Получаем параметры окна и делаем контекст текущим.
    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    assert(madeCurrent);

    display_ = display;
    surface_ = surface;
    context_ = context;

    // Делаем размеры окна невалидными,
    // чтобы они гарантированно обновились в первом кадре
    // внутри updateRenderArea().
    width_ = -1;
    height_ = -1;

    PRINT_GL_STRING(GL_VENDOR);
    PRINT_GL_STRING(GL_RENDERER);
    PRINT_GL_STRING(GL_VERSION);
    PRINT_GL_STRING_AS_LIST(GL_EXTENSIONS);

    shader_ = std::unique_ptr<Shader>(
            Shader::loadShader(vertex, fragment, "inPosition", "inUV", "uProjection", "objPosition", "objScale"));
    assert(shader_);

    // В демо используется только один шейдер,
    // поэтому активируем его сразу.
    // В более сложном проекте стоит отслеживать
    // текущий активный шейдер и переключать его при необходимости.
    shader_->activate();

    // Настраиваем глобальные состояния OpenGL.
    glClearColor(CORNFLOWER_BLUE);

    // Временно включаем поддержку альфа-смешивания глобально.
    // В реальной игре обычно так делать не стоит.
    glEnable(GL_BLEND);
//    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::updateRenderArea() {
    EGLint width;
    eglQuerySurface(display_, surface_, EGL_WIDTH, &width);

    EGLint height;
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);

    // Если размер поверхности изменился...
    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);

        // Помечаем проекционную матрицу как устаревшую,
        // чтобы она была пересоздана перед следующим рендерингом.
        shaderNeedsNewProjectionMatrix_ = true;
    }
}

int Renderer::getWidth() const
{
    return width_;
}

int Renderer::getHeight() const
{
    return height_;
}

float Renderer::getRatio() const
{
    return float(width_) / height_;
}

float Renderer::getProjectionHalfHeight() const
{
    return kProjectionHalfHeight;
}
