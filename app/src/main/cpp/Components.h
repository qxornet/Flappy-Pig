//
// Created by ilya on 26.06.2026.
//

#ifndef PUPPY_BIRD_COMPONENTS_H
#define PUPPY_BIRD_COMPONENTS_H

#include "Global.h"
#include <vector>

struct MeshComponent {
    MeshComponent() {}
    MeshComponent(std::vector<Vertex> &vertices, std::vector<Index> &indices)
    {
        indexCount = (GLsizei)indices.size();

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                vertices.data(),
                GL_STATIC_DRAW
        );

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(Index),
                indices.data(),
                GL_STATIC_DRAW
        );
    }

    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};

struct MaterialComponent {
    MaterialComponent(){}
    MaterialComponent(std::shared_ptr<TextureAsset> spTexture)
    : texture(std::move(spTexture)) {}

    std::shared_ptr<TextureAsset> texture;
};

struct TransformComponent
{
    Vector3 position{0, 0, 0};
    Vector3 rotation{0, 0, 0};
    Vector3 scale{1, 1, 1};
};

struct ColliderComponent
{
    Vector2 size;
};

struct MovementComponent
{
    Vector3 velocity;
};

struct AnimationComponent
{
    int currentFrame;
    int frameCount;

    float frameTime;
    float timer;
};

struct HealthComponent
{
    int points;
    int max_points;
};

struct DamageComponent
{
    int value;
};

struct UltimateComponent
{
    float points;
    bool enable;
};

#endif //PUPPY_BIRD_COMPONENTS_H
