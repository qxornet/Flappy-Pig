#ifndef ANDROIDGLINVESTIGATIONS_RENDERER_H
#define ANDROIDGLINVESTIGATIONS_RENDERER_H

#include <EGL/egl.h>
#include <memory>

#include "Model.h"
#include "Shader.h"
#include "Components.h"

struct android_app;

class Renderer {
public:
    /*!
     * @param pApp указатель на android_app, которому принадлежит данный Renderer.
     * Используется для настройки OpenGL.
     */
    inline Renderer(android_app *pApp) :
            app_(pApp),
            display_(EGL_NO_DISPLAY),
            surface_(EGL_NO_SURFACE),
            context_(EGL_NO_CONTEXT),
            width_(0),
            height_(0),
            shaderNeedsNewProjectionMatrix_(true) {
        initRenderer();
    }

    virtual ~Renderer();

    /*!
     * Выполняет отрисовку всех моделей, зарегистрированных в рендерере.
     */
    void beginFrame();
    void draw(std::shared_ptr<MeshComponent> mesh, const TransformComponent& transform, const MaterialComponent& material);
    void endFrame();

private:
    /*!
     * Выполняет необходимую инициализацию OpenGL.
     * Измените этот метод, если требуется настроить EGL-контекст
     * или глобальные параметры приложения.
     */
    void initRenderer();

    /*!
     * @brief Каждый кадр необходимо проверять, не изменился ли размер
     * буфера кадра (framebuffer). При изменении размера обновляется viewport.
     */
    void updateRenderArea();

    android_app *app_;
    EGLDisplay display_;
    EGLSurface surface_;
    EGLContext context_;
    EGLint width_;
    EGLint height_;

    bool shaderNeedsNewProjectionMatrix_;

    std::unique_ptr<Shader> shader_;
};

#endif // ANDROIDGLINVESTIGATIONS_RENDERER_H