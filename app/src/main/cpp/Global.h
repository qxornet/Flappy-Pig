#ifndef FLAPPY_PIG_GLOBAL_H
#define FLAPPY_PIG_GLOBAL_H

#include "TextureAsset.h"

enum class GameState
{
    MainMenu,
    Playing,
    GameOver,
    Restart,
    Pause,
    Exit
};

union Vector4 {
    struct {
        float north, west, south, east;
    };
    float idx[4];
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

#endif //FLAPPY_PIG_GLOBAL_H
