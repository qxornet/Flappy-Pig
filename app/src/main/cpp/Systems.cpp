#include "Systems.h"
#include "AndroidOut.h"

#include <limits>
#include <cmath>

bool checkCollision(
        const ColliderComponent& a_collider,
        const TransformComponent& a_transform,
        const ColliderComponent& b_collider,
        const TransformComponent& b_transform)
{
    const float leftA   = a_transform.position.x + a_collider.box.west  * a_transform.scale.x;
    const float rightA  = a_transform.position.x + a_collider.box.east  * a_transform.scale.x;
    const float topA    = a_transform.position.y + a_collider.box.north * a_transform.scale.y;
    const float bottomA = a_transform.position.y + a_collider.box.south * a_transform.scale.y;

    const float leftB   = b_transform.position.x + b_collider.box.west  * b_transform.scale.x;
    const float rightB  = b_transform.position.x + b_collider.box.east  * b_transform.scale.x;
    const float topB    = b_transform.position.y + b_collider.box.north * b_transform.scale.y;
    const float bottomB = b_transform.position.y + b_collider.box.south * b_transform.scale.y;

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

void MovementSystem::update(float dt, World& world, GameState &state)
{
    if(state == GameState::GameOver) return;

    const float bgWidth = world.background.mesh->width * world.background.transform.scale.x;

    world.background.transform.position.x -= 0.001f;
    world.secondBackground.transform.position.x -= 0.001f;

    if (world.background.transform.position.x <= -bgWidth)
    {
        world.background.transform.position.x =
                world.secondBackground.transform.position.x + bgWidth;
    }

    if (world.secondBackground.transform.position.x <= -bgWidth)
    {
        world.secondBackground.transform.position.x =
                world.background.transform.position.x + bgWidth;
    }

    if(state != GameState::Playing) return;

    if(world.player.isEnable) {
        world.player.transform.position.x = -(world.borderX / 3);
        world.player.movement.velocity.y +=
                world.player.controller.targetDirection * world.player.movement.acceleration * dt;

        const float damping = 5.0f;
        if (world.player.controller.targetDirection == 0) {
            world.player.movement.velocity.y *= std::exp(-damping * dt);
        }

        world.player.movement.velocity.y = std::clamp(
                world.player.movement.velocity.y,
                -world.player.movement.maxSpeed,
                world.player.movement.maxSpeed
        );

        world.player.transform.position.y += world.player.movement.velocity.y * dt;
        if (world.player.transform.position.y >= world.borderY - 0.2f)
            world.player.transform.position.y = world.borderY - 0.2f;

        if (world.player.transform.position.y <= -world.borderY + 0.2f)
            world.player.transform.position.y = -world.borderY + 0.2f;
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
            bomber.transform.position.y = (1.0f + (float)(rand() % 500)/1000.0f);
        }

        // обновляем стартовую позицию бомбы, пока она невидима
        if(!bomber.bomb.isShow) {
            bomber.bomb.transform.position = bomber.transform.position;
            continue;
        }

        bomber.bomb.transform.position.x += dt *  bomber.movement.velocity.x;
        bomber.bomb.transform.position.y -= dt *  bomber.bomb.movement.velocity.y;
    }

    for (auto& fighter : world.fighters)
    {
        if (!fighter.isShow || !fighter.isEnable)
            continue;

        fighter.transform.position.x -= fighter.movement.velocity.x * dt;

        if (fighter.transform.position.x > world.player.transform.position.x)
        {
            float dy = world.player.transform.position.y - fighter.transform.position.y;

            constexpr float followSpeed = 2.0f;

            fighter.movement.velocity.y = std::clamp(
                    dy * followSpeed,
                    -fighter.movement.maxSpeed,
                    fighter.movement.maxSpeed);
        }

        fighter.transform.position.y += fighter.movement.velocity.y * dt;

        if(-world.borderX-.2f > fighter.transform.position.x) {
            fighter.transform = fighter.transform_default;
            fighter.transform.position.y = (0.0f + (float)(rand() % 500)/1000.0f);
        }
    }

    for(auto &bullet : world.enemyBullets)
    {
        if(!bullet.isEnable || !bullet.isShow)
            continue;

        bullet.transform.position.x -= dt * bullet.movement.velocity.x;

        if(bullet.transform.position.x > -world.borderX-0.5f) continue;
        bullet.isEnable = false;
        bullet.isShow = false;
    }

    for(auto &bullet : world.playerBullets)
    {
        if(!bullet.isEnable || !bullet.isShow)
            continue;

        bullet.transform.position.x += dt * bullet.movement.velocity.x;

        if(bullet.transform.position.x < world.borderX+0.5f) continue;
        bullet.isEnable = false;
        bullet.isShow = false;
    }

//    world.meteor.transform.position.x += dt * world.meteor.movement.velocity.x;
//    world.meteor.transform.position.y += dt * world.meteor.movement.velocity.y;
}

void CollisionSystem::update(float dt, World& world, GameState &state)
{
    if(state != GameState::Playing) return;

    if (world.player.transform.position.y <= -world.borderY + 0.2f) {
        world.player.health.points = 0;
    }

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

        if(checkCollision(bird.collision, bird.transform,
                          world.player.collision, world.player.transform)) {
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

        if(checkCollision(bomber.collision, bomber.transform,
                                    world.player.collision, world.player.transform)) {
            world.player.health.points = 0;
            bomber.health.points = 0;
            bomber.animation.current = AnimationType::Die;
        };

        for(auto &bullet : world.playerBullets)
        {
            if(!bullet.isEnable || !bullet.isShow) continue;
            if(checkCollision(bomber.collision, bomber.transform,
                              bullet.collision, bullet.transform)) {
                bomber.health.points = 0;
                bomber.animation.current = AnimationType::Die;
                bullet.isEnable = false;
                bullet.isShow = false;
                world.player.score.count += bomber.cost.count;
            };
        }
    }

    for (auto& fighter : world.fighters)
    {
        if (!fighter.isShow || !fighter.isEnable)
            continue;

        float dist = distanceToPlayer(
                fighter.collision,
                fighter.transform,
                world.player);

        if (dist > 1.0f)
            fighter.animation.current = AnimationType::Run;
//        else
//            fighter.animation.current = AnimationType::Danger;

        if(checkCollision(fighter.collision, fighter.transform,
                                    world.player.collision, world.player.transform)) {
            world.player.health.points = 0;
            fighter.health.points = 0;
            fighter.animation.current = AnimationType::Die;
        };

        for(auto &bullet : world.playerBullets)
        {
            if(!bullet.isEnable || !bullet.isShow) continue;
            if(checkCollision(fighter.collision, fighter.transform,
                              bullet.collision, bullet.transform)) {
                fighter.health.points = 0;
                fighter.animation.current = AnimationType::Die;
                bullet.isEnable = false;
                bullet.isShow = false;
                world.player.score.count += fighter.cost.count;
            };
        }
    }

    for(auto &bullet : world.enemyBullets)
    {
        if(checkCollision(bullet.collision, bullet.transform,
                                    world.player.collision, world.player.transform)) {
            world.player.health.points = 0;
            bullet.isEnable = false;
            bullet.isShow = false;
        };
    }
}

void InputSystem::update(float dt, World& world, GameState &state)
{
    if(state != GameState::Playing) return;

    world.player.attack.intervalTimer += dt;
    if(world.player.attack.shooting)
    {
        if(world.player.attack.intervalTimer >= world.player.attack.interval) {
            if(world.player.attack.count >= world.player.attack.maxCount)
                world.player.attack.count = 0;

            int bulletIdx = world.player.attack.count;
            world.playerBullets[bulletIdx].transform.position = world.player.transform.position;
            world.playerBullets[bulletIdx].transform.position.y -= 0.06f;
            world.playerBullets[bulletIdx].isEnable = true;
            world.playerBullets[bulletIdx].isShow = true;
            world.player.attack.intervalTimer = 0;
            world.player.attack.count++;
        }
    }
}

void SpawnSystem::update(float dt, World& world, GameState &state)
{
    if(state != GameState::Playing) return;

    float birdSpawnAccum = 0.0f;
    for(auto &bird : world.birds) {
        birdSpawnAccum += bird.spawn.spawnTime;
        if(bird.isEnable || bird.isShow) continue;

        bird.spawn.timer += dt;
        if(bird.spawn.timer >= birdSpawnAccum) {
            bird.isEnable = true;
            bird.isShow = true;
            bird.movement.velocity.y = -0.4f + (float)(rand() % 800) / 1000.0f; // [-0.4; 0.4]
        }

        if(!bird.health.points) {
            bird.health = bird.health_default;
        }
    }

    float bomberSpawnAccum = 0.0f;
    for(auto &bomber : world.bombers) {
        bomberSpawnAccum += bomber.spawn.spawnTime;
        if(bomber.isEnable || bomber.isShow) continue;

        bomber.spawn.timer += dt;
        if(bomber.spawn.timer >= bomberSpawnAccum) {
            bomber.isEnable = true;
            bomber.isShow = true;
            bomber.transform.position.y = (1.0f + (float)(rand() % 500)/1000.0f);
        }

        if(!bomber.health.points) {
            bomber.health = bomber.health_default;
        }
    }

    float fighterSpawnAccum = 0.0f;
    for(auto &fighter : world.fighters) {
        fighterSpawnAccum += fighter.spawn.spawnTime;
        if(fighter.isEnable || fighter.isShow) continue;

        fighter.spawn.timer += dt;
        if(fighter.spawn.timer >= fighterSpawnAccum) {
            fighter.isEnable = true;
            fighter.isShow = true;
            fighter.spawn.timer = 0;
            fighter.transform.position.y = (0.0f + (float)(rand() % 500)/1000.0f);
        }

        if(!fighter.health.points) {
            fighter.health = fighter.health_default;
        }
    }
}

void HealthSystem::update(float dt, World& world, GameState &state)
{
    if(state != GameState::Playing) return;

    if(world.player.health.points == 0) {
        world.player.isEnable = false;
        state = GameState::GameOver;
    }

    for(auto &bird : world.birds) {
        if(bird.health.points != 0) continue;
        bird.isShow = false;
        bird.isEnable = false;
        bird.health = bird.health_default;
        bird.movement = bird.movement_default;
        bird.transform = bird.transform_default;
    }

    for(auto &bomber : world.bombers) {
        if(bomber.health.points == 0) {
            bomber.isShow = false;
            bomber.isEnable = false;
            bomber.health = bomber.health_default;
            bomber.movement = bomber.movement_default;
            bomber.transform = bomber.transform_default;
        }

        // пока bomb имеет bomb.hasDestroy == false - она существует, в противном случае скрываем объект и делаем его недоступным
        bomber.bomb.isShow = !bomber.bomb.hasDestroy;
        bomber.bomb.isEnable = !bomber.bomb.hasDestroy;
    }

    for(auto &fighter : world.fighters) {
        if(fighter.health.points != 0) continue;
        fighter.isShow = false;
        fighter.isEnable = false;
        fighter.health = fighter.health_default;
        fighter.movement = fighter.movement_default;
        fighter.transform = fighter.transform_default;
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

void AnimationSystem::update(float dt, World& world, GameState &state)
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

    for (auto& fighter : world.fighters) {
        if (!fighter.isEnable || !fighter.isShow)
            continue;

        updateAnimation(
                dt,
                fighter.animation,
                fighter.material);
    }

    for (auto& bullet : world.playerBullets) {
        if (!bullet.isEnable || !bullet.isShow)
            continue;

        updateAnimation(
                dt,
                bullet.animation,
                bullet.material);
    }

    for (auto& bullet : world.enemyBullets) {
        if (!bullet.isEnable || !bullet.isShow)
            continue;

        updateAnimation(
                dt,
                bullet.animation,
                bullet.material);
    }
}

void EventSystem::update(float dt, World& world, GameState& state)
{
    if (state != GameState::Playing)
        return;

    for (auto& fighter : world.fighters)
    {
        if (!fighter.isEnable || !fighter.isShow)
            continue;

        auto& attack = fighter.attack;

        if (!attack.shooting)
        {
            attack.cooldownTimer += dt;

            if (attack.cooldownTimer < attack.cooldown)
                continue;

            attack.shooting = true;
            attack.cooldownTimer = 0.0f;
            attack.intervalTimer = attack.interval; // первая пуля сразу
            attack.count = 0;
        }

        attack.intervalTimer += dt;

        if (attack.intervalTimer < attack.interval)
            continue;

        attack.intervalTimer = 0.0f;

        for (auto& bullet : world.enemyBullets)
        {
            if (bullet.isEnable || bullet.isShow)
                continue;

            bullet.isEnable = true;
            bullet.isShow = true;

            bullet.transform.position = fighter.transform.position;

            ++attack.count;
            break;
        }

        // Очередь закончилась
        if (attack.count >= attack.maxCount)
        {
            attack.shooting = false;
            attack.count = 0;
            attack.cooldownTimer = 0.0f;
            attack.intervalTimer = 0.0f;
        }
    }
}

void UltimateSystem::update(float dt, World& world, GameState &state)
{
    if(state != GameState::Playing) return;

    // отвечает за событие ультимейта (способности) игрока: проверяет условия, по которым он станет доступен
    // развивая эту систему ее можно переименовать в SpellSystem
}


