#ifndef FLAPPY_PIG_UIELEMENTS_H
#define FLAPPY_PIG_UIELEMENTS_H

#include "Components.h"

struct Button
{
    std::shared_ptr<MeshComponent> mesh;
    TransformComponent transform;
    MaterialComponent material;

    std::function<void()> onClick;
};

std::vector<Button> initMainMenu(AAssetManager *assetManager, GameState *state);
std::vector<Button> initGameoverMenu(AAssetManager *assetManager, GameState *state);
Button initAttackControl(AAssetManager *assetManager, bool *enableAttack);

#endif //FLAPPY_PIG_UIELEMENTS_H
