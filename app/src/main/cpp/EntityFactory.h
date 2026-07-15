#ifndef FLAPPY_PIG_ENTITYFACTORY_H
#define FLAPPY_PIG_ENTITYFACTORY_H

#include "Entity.h"

class EntityFactory
{
public:
    EntityFactory(const std::unordered_map<EntityId, std::shared_ptr<MeshComponent>>& meshes,
            const std::unordered_map<EntityId, AnimationComponent>& textures) : meshStorage(meshes), textureStorage(textures) {}

    Background createBackground();
    Player createPlayer();
    Bird createBird();
    Bomber createBomber();
    Fighter createFighter();
    Bullet createEnemyBullet();
    Bullet createPlayerBullet();
    Meteor createMeteor();

private:
    void setupRenderable(Renderable& renderable, EntityId id, AnimationType animation, int idAnimation = 0);
    void setupEntity(IEntity& entity, EntityId id);

private:
    const std::unordered_map<EntityId, std::shared_ptr<MeshComponent>>& meshStorage;
    const std::unordered_map<EntityId, AnimationComponent>& textureStorage;
};

#endif //FLAPPY_PIG_ENTITYFACTORY_H
