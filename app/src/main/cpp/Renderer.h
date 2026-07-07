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

    void beginFrame();
    void draw(std::shared_ptr<MeshComponent> mesh, const TransformComponent& transform, const MaterialComponent& material);
    void endFrame();

    int getHeight() const;
    int getWidth() const;

    float getRatio() const;
    float getProjectionHalfHeight() const;

private:
    void initRenderer();
    void updateRenderArea();

private:
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