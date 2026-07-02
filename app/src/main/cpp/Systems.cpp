#include "Systems.h"

void MovementSystem::update(float dt, World& world)
{
    for(auto& bird : world.birds)
    {
        bird.transform.position.x += dt *  bird.movement.velocity.x;
    }

    for(auto& bomber : world.bombers)
    {
        if(!bomber.isShow || !bomber.isEnable) continue;
        // добавить проверку, если самолет ушел за границы карты - в таком случае reset для позиции и текстуры
        bomber.transform.position.x += dt *  bomber.movement.velocity.x;

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
        if(world.player.collision.box.east ==  bird.collision.box.west)
            world.player.health.points = 0;
    }

    for(auto &bomber : world.bombers)
    {
        if(world.player.collision.box.east ==  bomber.collision.box.west)
            world.player.health.points = 0;

        // проверяем, если объект доступен, то коллизию можно проверять
        if(!bomber.bomb.isEnable) continue;
        if(world.player.collision.box.east ==  bomber.bomb.collision.box.west ||
           world.player.collision.box.south ==  bomber.bomb.collision.box.north) {
            // добавить доп. условие на проверку достижения земли, если достигла - мы задаем health = 0 и объект уничтожается
            world.player.health.points = 0;
            bomber.bomb.hasDestroy = true;
        }
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
    if(world.player.health.points == 0); // call end game event

    for(auto &bird : world.birds) {
        if(bird.health.points == 0) {
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

void AnimationSystem::update(float dt, World& world)
{
    // ,eltn pf,fdyj tckb ds 'nj ghjxntnt
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


