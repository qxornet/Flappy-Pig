
#ifndef FLAPPY_PIG_SCORE_H
#define FLAPPY_PIG_SCORE_H

#include "Renderer.h"
#include "Entity.h"

class ScoreInfo
{
public:

    void draw(Renderer& renderer, int value);

    std::shared_ptr<MeshComponent> digitMesh;
    std::array<Digit, 6> digits;
    std::vector<std::shared_ptr<TextureAsset>> textures;
};


#endif //FLAPPY_PIG_SCORE_H
