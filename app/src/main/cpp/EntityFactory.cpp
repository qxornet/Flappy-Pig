#include "EntityFactory.h"

void EntityFactory::setupRenderable(
        Renderable& renderable,
        EntityId id,
        AnimationType animation,
        int idAnimation)
{
    renderable.mesh = meshStorage.at(id);

    renderable.material.texture =
            textureStorage.at(id).clips.at(animation)[idAnimation];

    renderable.material_default =
            renderable.material;

    renderable.isShow = false;
}

void EntityFactory::setupEntity(
        IEntity& entity,
        EntityId id)
{
    entity.id = id;
    entity.isEnable = false;
    entity.transform = {0.0f, 0.0f, 0.0f};
    entity.transform_default = entity.transform;
}

Background EntityFactory::createBackground()
{
    Background background;
    setupEntity(background, EntityId::eBackground);
    setupRenderable(background, EntityId::eBackground, AnimationType::Idle);
    background.isShow = true;
    background.transform.setScale(2.5f);
    background.transform.position.x = 2.5f;
    background.transform.position.y = 0.5f;
    return background;
}

Player EntityFactory::createPlayer()
{
    Player player;
    setupEntity(player, EntityId::ePlayer);
    setupRenderable(player, EntityId::ePlayer, AnimationType::Run);

    player.transform.setScale(.4f);
    player.transform.position.x = -0.5f;

    player.health.points = 100;
    player.health.max_points = 100;

    player.movement.maxSpeed = 2.5f;
    player.movement.velocity.y = 1.0f;
    player.movement.acceleration = 10.0f;

    player.animation.clips = textureStorage.at(EntityId::ePlayer).clips;
    player.animation.frameTime = 0.150f;

    player.collision.box = {0.3f, -0.45f, -0.3f, 0.45f};

    player.isEnable = true;
    player.isShow = true;

    player.attack.count = 0;
    player.attack.maxCount = 100;
    player.attack.interval = 0.250f;
    player.attack.cooldown = 1.500f;

    player.movement_default = player.movement;
    player.transform_default = player.transform;
    player.health_default = player.health;
    player.material_default = player.material;

    return player;
}

Bird EntityFactory::createBird()
{
    Bird bird;

    setupEntity(bird, EntityId::eBird);
    setupRenderable(bird, EntityId::eBird, AnimationType::Run);

    bird.transform.setScale(0.15f);
    bird.transform.position.z = 2.0f;
    bird.transform.position.x = 5.0f;

    bird.health = {1, 1};

    bird.movement.maxSpeed = 1.5f;
    bird.movement.velocity.x = 0.3f;

    bird.animation.clips = textureStorage.at(EntityId::eBird).clips;
    bird.animation.frameTime = 0.250f;

    bird.collision.box = {0.3f, -0.45f, -0.3f, 0.45f};

    bird.movement_default = bird.movement;
    bird.transform_default = bird.transform;
    bird.health_default = bird.health;
    bird.material_default = bird.material;

    bird.spawn.spawnTime = 15.0f;

    return bird;
}

Bomber EntityFactory::createBomber()
{
    Bomber bomber;

    setupEntity(bomber, EntityId::eBomber);
    setupRenderable(bomber, EntityId::eBomber, AnimationType::Run);

    bomber.transform.setScale(0.2f);
    bomber.transform.position.z = 2.5f;
    bomber.transform.position.x = 5.0f;
    bomber.transform.position.y = 1.0f;

    bomber.health = {1, 1};

    bomber.movement.maxSpeed = 1.5f;
    bomber.movement.velocity.x = 0.6f;

    bomber.animation.clips = textureStorage.at(EntityId::eBomber).clips;
    bomber.animation.frameTime = 0.250f;

    bomber.collision.box = {0.35f, -0.55f, -0.35f, 0.55f};

    bomber.movement_default = bomber.movement;
    bomber.transform_default = bomber.transform;
    bomber.health_default = bomber.health;
    bomber.material_default = bomber.material;

    bomber.spawn.spawnTime = 25.0f;


    return bomber;
}

Fighter EntityFactory::createFighter()
{
    Fighter fighter;

    setupEntity(fighter, EntityId::eFighter);
    setupRenderable(fighter, EntityId::eFighter, AnimationType::Run);

    fighter.transform.setScale(0.2f);
    fighter.transform.position.z = 2.5f;
    fighter.transform.position.x = 5.0f;

    fighter.health = {1, 1};

    fighter.movement.maxSpeed = 1.5f;
    fighter.movement.velocity.x = 0.6f;

    fighter.animation.clips = textureStorage.at(EntityId::eFighter).clips;
    fighter.animation.frameTime = 0.080f;

    fighter.collision.box = {0.25f, -0.25f, -0.10f, 0.15f};

    fighter.attack.maxCount = 3;
    fighter.attack.cooldown = 1.500f;
    fighter.attack.interval = 0.250f;

    fighter.movement_default = fighter.movement;
    fighter.transform_default = fighter.transform;
    fighter.health_default = fighter.health;
    fighter.material_default = fighter.material;

    fighter.spawn.spawnTime = 30.0f;

    return fighter;
}

Bullet EntityFactory::createEnemyBullet()
{
    Bullet bullet;
    setupEntity(bullet, EntityId::eEnemyBullet);
    setupRenderable(bullet, EntityId::eEnemyBullet, AnimationType::Run);

    bullet.transform.setScale(.1f);
    bullet.movement.velocity.x = 1.0f;

    bullet.animation.clips = textureStorage.at(EntityId::eEnemyBullet).clips;
    bullet.animation.frameTime = 0.150f;

    bullet.collision.box = {0.3f, -0.45f, -0.3f, 0.45f};

    bullet.transform_default = bullet.transform;
    bullet.material_default = bullet.material;

    return bullet;
}

Bullet EntityFactory::createPlayerBullet()
{
    Bullet bullet;
    setupEntity(bullet, EntityId::ePlayerBullet);
    setupRenderable(bullet, EntityId::ePlayerBullet, AnimationType::Run);

    bullet.transform.setScale(.1f);
    bullet.movement.velocity.x = 1.0f;

    bullet.animation.clips = textureStorage.at(EntityId::ePlayerBullet).clips;
    bullet.animation.frameTime = 0.150f;

    bullet.collision.box = {0.3f, -0.45f, -0.3f, 0.45f};

    bullet.transform_default = bullet.transform;
    bullet.material_default = bullet.material;

    return bullet;
}

Meteor EntityFactory::createMeteor()
{

}