//
// Created by ilya on 26.06.2026.
//

#ifndef PUPPY_BIRD_GLOBAL_H
#define PUPPY_BIRD_GLOBAL_H

#include "TextureAsset.h"

enum class EntityId
{
    eBackground,
    ePlayer,
    eBird,
    eBomber,
    eBomb,
    eFighter,
    eBuilding,
    eTerrain,
    eMeteor
};

union Vector3 {
    struct {
        float x, y, z;
    };
    float idx[3];
};

union Vector2 {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
    float idx[2];
};

struct Vertex {
    constexpr Vertex(const Vector3 &inPosition, const Vector2 &inUV) : position(inPosition),
                                                                       uv(inUV) {}

    Vector3 position;
    Vector2 uv;
};

typedef uint16_t Index;


#endif //PUPPY_BIRD_GLOBAL_H
