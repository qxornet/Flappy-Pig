#include "Systems.h"
#include "AndroidOut.h"

#include <limits>
#include <cmath>

bool checkCollisionWithPlayer(
        const ColliderComponent& collider,
        const TransformComponent& transform,
        const Player& player)
{
    const auto& playerBox = player.collision.box;
    const auto& playerTransform = player.transform;

    // Границы текущего объекта
    const float leftA   = transform.position.x + collider.box.west  * transform.scale.x;
    const float rightA  = transform.position.x + collider.box.east  * transform.scale.x;
    const float topA    = transform.position.y + collider.box.north * transform.scale.y;
    const float bottomA = transform.position.y + collider.box.south * transform.scale.y;

    // Границы игрока
    const float leftB   = playerTransform.position.x + playerBox.west  * playerTransform.scale.x;
    const float rightB  = playerTransform.position.x + playerBox.east  * playerTransform.scale.x;
    const float topB    = playerTransform.position.y + playerBox.north * playerTransform.scale.y;
    const float bottomB = playerTransform.position.y + playerBox.south * playerTransform.scale.y;

    // AABB-пересечение
    return !(leftA > rightB ||
             rightA < leftB ||
             bottomA > topB ||
             topA < bottomB);
}

float distanceToPlayer(
        const ColliderComponent& collider,
        const TransformComponent& transform,
        const Player& player)
{
    const auto& playerBox = player.collision.box;
    const auto& playerTransform = player.transform;

    const float topA =
            transform.position.y + collider.box.north * transform.scale.y;
    const float bottomA =
            transform.position.y + collider.box.south * transform.scale.y;

    const float topB =
            playerTransform.position.y + playerBox.north * playerTransform.scale.y;
    const float bottomB =
            playerTransform.position.y + playerBox.south * playerTransform.scale.y;

    if (bottomA > topB || topA < bottomB)
        return std::numeric_limits<float>::infinity();

    return std::abs(transform.position.x - playerTransform.position.x);
}

void MovementSystem::update(float dt, World& world)
{
    if(world.player.isEnable) {
        world.player.transform.position.x = -(world.borderX / 3);
        world.player.movement.velocity.y +=
                world.player.inputY * world.player.movement.acceleration * dt;

        const float damping = 5.0f;
        if (world.player.inputY == 0) {
            world.player.movement.velocity.y *= std::exp(-damping * dt);
        }

        world.player.movement.velocity.y = std::clamp(
                world.player.movement.velocity.y,
                -world.player.movement.maxSpeed,
                world.player.movement.maxSpeed
        );

        world.player.transform.position.y += world.player.movement.velocity.y * dt;
        if (world.player.transform.position.y >= world.borderY - world.player.transform.scale.y)
            world.player.transform.position.y = world.borderY - world.player.transform.scale.y;

        if (world.player.transform.position.y <= -world.borderY + world.player.transform.scale.y)
            world.player.transform.position.y = -world.borderY + world.player.transform.scale.y;
    }

    const float minBirdY = -world.borderY + 0.2f;
    const float maxBirdY = -0.8f;
    for (auto& bird : world.birds)
    {
        if (!bird.isShow || !bird.isEnable)
            continue;

        bird.transform.position.x -= dt * bird.movement.velocity.x;
        bird.transform.position.y += dt * bird.movement.velocity.y;

        if ((rand() % 100) == 0)
        {
            bird.movement.velocity.y =
                    -0.4f + (float)(rand() % 800) / 1000.0f; // [-0.4; 0.4]
        }

        bird.transform.position.y = std::clamp(
                bird.transform.position.y,
                minBirdY,
                maxBirdY
        );

        if (bird.transform.position.y >= maxBirdY)
        {
            bird.transform.position.y = maxBirdY;
            bird.movement.velocity.y = -std::abs(bird.movement.velocity.y);
        }

        if (bird.transform.position.y <= minBirdY)
        {
            bird.transform.position.y = minBirdY;
            bird.movement.velocity.y = std::abs(bird.movement.velocity.y);
        }

        if (bird.transform.position.x <= -world.borderX - 0.5f)
        {
            bird.transform.position.x = world.borderX + 0.5f;
            bird.transform.position.y =
                    -1.0f + (float)(rand() % 2000) / 1000.0f; // [-1; 1]

            bird.movement.velocity.y =
                    -0.4f + (float)(rand() % 800) / 1000.0f;
        }
    }

    for(auto& bomber : world.bombers)
    {
        if(!bomber.isShow || !bomber.isEnable) continue;
        bomber.transform.position.x -= dt *  bomber.movement.velocity.x;
        if(-world.borderX-.2f > bomber.transform.position.x) {
            bomber.transform = bomber.transform_default;
            bomber.transform.position.y = (1.0f + (float)(rand() % 500)/1000.0f); // диапазон 0.5f до 1.0f
        }

        // обновляем стартовую позицию бомбы, пока она невидима
        if(!bomber.bomb.isShow) {
            bomber.bomb.transform.position = bomber.transform.position;
            continue;
        }

        bomber.bomb.transform.position.x += dt *  bomber.movement.velocity.x;
        bomber.bomb.transform.position.y -= dt *  bomber.bomb.movement.velocity.y;
    }

    for(auto& fighter : world.fighters)
    {
        // добавить условие (раз в n времен истребитель струляет тремя bullets)
        fighter.transform.position.x += dt *  fighter.movement.velocity.x;
    }

    world.meteor.transform.position.x += dt * world.meteor.movement.velocity.x;
    world.meteor.transform.position.y += dt * world.meteor.movement.velocity.y;
}

void CollisionSystem::update(float dt, World& world)
{
    for(auto &bird : world.birds)
    {
        if(!bird.isShow || !bird.isEnable) continue;

        float dist = distanceToPlayer(
                bird.collision,
                bird.transform,
                world.player);

        if (dist > 0.6f)
            bird.animation.current = AnimationType::Run;
        else if (dist > 0.5f)
            bird.animation.current = AnimationType::DangerFar;
        else if (dist > 0.4f)
            bird.animation.current = AnimationType::DangerMid;
        else
            bird.animation.current = AnimationType::DangerNear;

        if(checkCollisionWithPlayer(bird.collision, bird.transform, world.player)) {
            world.player.health.points = 0;
            bird.health.points = 0;
            bird.animation.current = AnimationType::Die;
        };
    }

    for(auto &bomber : world.bombers)
    {
        if(!bomber.isShow || !bomber.isEnable) continue;

        float dist = distanceToPlayer(
                bomber.collision,
                bomber.transform,
                world.player);

        if (dist > 1.0f)
            bomber.animation.current = AnimationType::Run;
        else
            bomber.animation.current = AnimationType::Danger;

        if(checkCollisionWithPlayer(bomber.collision, bomber.transform, world.player)) {
            world.player.health.points = 0;
            bomber.health.points = 0;
            bomber.animation.current = AnimationType::Die;
        };

//        if(world.player.collision.box.east ==  bomber.collision.box.west)
//            world.player.health.points = 0;

//        // проверяем, если объект доступен, то коллизию можно проверять
//        if(!bomber.bomb.isEnable) continue;
//        if(world.player.collision.box.east ==  bomber.bomb.collision.box.west ||
//           world.player.collision.box.south ==  bomber.bomb.collision.box.north) {
//            // добавить доп. условие на проверку достижения земли, если достигла - мы задаем health = 0 и объект уничтожается
//            world.player.health.points = 0;
//            bomber.bomb.hasDestroy = true;
//        }
    }

    for(auto &fighter : world.fighters)
    {
        if(world.player.collision.box.east ==  fighter.collision.box.west) {
            world.player.health.points = 0;
            fighter.health.points = 0;
        }
    }
}

void InputSystem::update(float dt, World& world)
{

}

void SpawnSystem::update(float dt, World& world)
{
    // при спавне нужно задать рандомную стартовую анимацию и сделать ресет объектам
    // необходимо определить условия спавна различных entity на карте
}

void HealthSystem::update(float dt, World& world)
{
    if(world.player.health.points == 0)
        world.player.isEnable = false; // call end game event

    for(auto &bird : world.birds) {
        if(bird.health.points == 0) {
            bird.isShow = false;
            bird.isEnable = false;
            bird.health = bird.health_default;
            bird.movement = bird.movement_default;
            bird.transform = bird.transform_default;
        }
    }

    for(auto &bomber : world.bombers) {
        if(bomber.health.points == 0) {
            bomber.health = bomber.health_default;
            bomber.movement = bomber.movement_default;
            bomber.transform = bomber.transform_default;
        }

        // пока bomb имеет bomb.hasDestroy == false - она существует, в противном случае скрываем объект и делаем его недоступным
        bomber.bomb.isShow = !bomber.bomb.hasDestroy;
        bomber.bomb.isEnable = !bomber.bomb.hasDestroy;
    }
}

void updateAnimation(
        float dt,
        AnimationComponent& animation,
        MaterialComponent& material)
{
    animation.timer += dt;

    if (animation.timer < animation.frameTime)
        return;

    animation.timer -= animation.frameTime;

    auto it = animation.clips.find(animation.current);
    if (it == animation.clips.end() || it->second.empty())
        return;

    auto& frames = it->second;

    animation.frame = (animation.frame + 1) % frames.size();
    material.texture = frames[animation.frame];
}


void AnimationSystem::update(float dt, World& world)
{
    if(world.player.isEnable && world.player.isShow) {
        updateAnimation(
                dt,
                world.player.animation,
                world.player.material);
    }

    for (auto& bird : world.birds) {
        if (!bird.isEnable || !bird.isShow)
            continue;

        updateAnimation(
                dt,
                bird.animation,
                bird.material);
    }

    for (auto& bomber : world.bombers) {
        if (!bomber.isEnable || !bomber.isShow)
            continue;

        updateAnimation(
                dt,
                bomber.animation,
                bomber.material);
    }
}

void EventSystem::update(float dt, World& world)
{
    // отвечает за события появления бомб, метеоритов или bullets
    // но смотря какие fabric, смотря сколько details
}

void UltimateSystem::update(float dt, World& world)
{
    // отвечает за событие ультимейта (способности) игрока: проверяет условия, по которым он станет доступен
    // развивая эту систему ее можно переименовать в SpellSystem
}


