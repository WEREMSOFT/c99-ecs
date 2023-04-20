#ifndef __ENTITIES_CREATORS__
#define __ENTITIES_CREATORS__

#include "tilemap.h"

// TREE
void registryAddTree(Registry* _this, int x, int y, Vector2 scaleV) 
{
	static float phase;
	phase += .3;
	int entityId = registryCreateEntity(_this);
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TREE, 16, 32, 10, 0, 0, 2.);
	entityAddComponent(entityId, _this, &spriteComponent, COMPONENT_SPRITE);
	TransformComponent transformComponent = {{10, 10}, scaleV, 0};
	entityAddComponent(entityId, _this, &transformComponent, COMPONENT_TRANSFORM);
	CircularMovementComponent cmc = {.phase = phase, .center = {(x) * 100., (y) * 100.}, .radius = 100. };
	entityAddComponent(entityId, _this, &cmc, COMPONENT_CIRCULAR_MOVEMENT);
}
// BULLET
void registryAddBullet(Registry* _this, int x, int y, Vector2 scaleV, Vector2 velocity) 
{
	int entityId = registryCreateEntity(_this);
	
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_BULLET, 4, 4, 10, 0, 0, 2.);
	entityAddComponent(entityId, _this, &spriteComponent, COMPONENT_SPRITE);
	
	TransformComponent transformComponent = {{x, y}, scaleV, 0};
	entityAddComponent(entityId, _this, &transformComponent, COMPONENT_TRANSFORM);

	RigidBodyComponent rigidBodyComponent = {0};
	rigidBodyComponent.velocity = velocity;
	entityAddComponent(entityId, _this, &rigidBodyComponent, COMPONENT_RIGID_BODY);
}

//HELICOPTER
void helicopterCreate(Registry* registry, Vector2 scaleV)
{
	int entityId = registryCreateEntity(registry);
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_CHOPPER, 32, 32, 10, 0, 0, 2.);
	entityAddComponent(entityId, registry, &spriteComponent, COMPONENT_SPRITE);
	TransformComponent transformComponent = {{100, 100}, scaleV, 0};
	entityAddComponent(entityId, registry, &transformComponent, COMPONENT_TRANSFORM);

	entityAddTag(entityId, registry, TAG_PLAYER);

	KeyboardComponent keyboardComponent = {.velocity = 80.};
	entityAddComponent(entityId, registry, &keyboardComponent, COMPONENT_KEYBOARD_CONTROLLER);

	RigidBodyComponent rigidBodyComponent = {0};
	entityAddComponent(entityId, registry, &rigidBodyComponent, COMPONENT_RIGID_BODY);

	AnimationComponent animationComponent = animationComponentCreate(2, 7, true);
	entityAddComponent(entityId, registry, &animationComponent, COMPONENT_ANIMATION);

	ProjectileEmitterComponent projectileComponent = projectileEmitterCreate((Vector2){100., 0}, 20., 10., 10., true);
	entityAddComponent(entityId, registry, &projectileComponent, COMPONENT_PROJECTILE_EMITTER);
}

#endif