#ifndef PUPPY_BIRD_COMPONENTS_H
#define PUPPY_BIRD_COMPONENTS_H

#include "Global.h"
#include <vector>
#include <map>
#include <string>

enum class AnimationType
{
    Idle,
    Run,
    Danger,
    DangerFar,
    DangerMid,
    DangerNear,
    Attack,
    Die
};

struct MeshComponent {
    MeshComponent() = default;
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

    void setScale(float e_scale) {
        scale = {e_scale, e_scale, e_scale};
    }
};

struct EventComponent // для управления ивентовыми событиями
{
    bool enable;
    float lastEvent;
};

struct PlayerControllerComponent
{
    float targetDirection = 0.0f;
};

struct ColliderComponent
{
    Vector4 box;
};

struct MovementComponent
{
    Vector3 velocity = {0, 0, 0};
    float maxSpeed = 0;
    float acceleration = 1.0f;
};

struct AnimationComponent
{
    AnimationType current = AnimationType::Run;

    int frame = 0;
    float frameTime = 0;
    float timer = 0;

    std::unordered_map<AnimationType, std::vector<std::shared_ptr<TextureAsset>>> clips;
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
