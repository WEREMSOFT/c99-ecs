#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"
#include "core/logger.h"

#include <SDL2/SDL.h>
#include "game.h"

int main()
{
	float scale = 1.;
	Vector2 scaleV = {scale, scale};
	Registry registry = registryCreate();
	// Entity 1
	{
		int entityId = entityCreate(&registry);

		SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TREE, 16, 32, 0, 0, 0, 2.);
		entityAddComponent(entityId, &registry, &spriteComponent, COMPONENT_SPRITE);
		
		TransformComponent transformComponent = {{10, 15}, scaleV, 0};
		entityAddComponent(entityId, &registry, &transformComponent, COMPONENT_TRANSFORM);

		CircularMovementComponent cmc = {.phase = 0., .center = {100., 100.}, .radius = 100. };

		entityAddComponent(entityId, &registry, &cmc, COMPONENT_CIRCULAR_MOVEMENT);
	}
	// Entity 2
	{
		int entityId = entityCreate(&registry);

		SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TANK_TIGER_UP, 32, 32, 0, 0, 0, 2.);
		entityAddComponent(entityId, &registry, &spriteComponent, COMPONENT_SPRITE);
		
		TransformComponent transformComponent = {{20, 25}, scaleV, 0};
		entityAddComponent(entityId, &registry, &transformComponent, COMPONENT_TRANSFORM);

		CircularMovementComponent cmc = {.phase = 0., .center = {200., 100.}, .radius = 100. };

		entityAddComponent(entityId, &registry, &cmc, COMPONENT_CIRCULAR_MOVEMENT);
	}
	// Entity 3
	{
		int entityId = entityCreate(&registry);

		SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_LANDING_BASE, 32, 32, 0, 0, 0, 2.);
		entityAddComponent(entityId, &registry, &spriteComponent, COMPONENT_SPRITE);
		
		TransformComponent transformComponent = {{30, 35}, scaleV, 0};
		entityAddComponent(entityId, &registry, &transformComponent, COMPONENT_TRANSFORM);

		CircularMovementComponent cmc = {.phase = 0., .center = {300., 100.}, .radius = 100. };

		entityAddComponent(entityId, &registry, &cmc, COMPONENT_CIRCULAR_MOVEMENT);
	}

	registry = registryUpdate(registry);

	for(int i = 0; i < registry.entityCount; i++)
	{
		TransformComponent* tc = entityGetComponent(i, registry, COMPONENT_TRANSFORM);
		printf("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	}
	printf("#########\n");

	for(int i = 0; i < registry.components[COMPONENT_TRANSFORM]->size; i++)
	{
		TransformComponent* tc = arrayGetElementAt(registry.components[COMPONENT_TRANSFORM], i);
		printf("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	}

	entityDelete(1, &registry);

	for(int i = 0; i < registry.entityCount; i++)
	{
		TransformComponent* tc = entityGetComponent(i, registry, COMPONENT_TRANSFORM);
		printf("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	}
	printf("#########\n");

	for(int i = 0; i < registry.components[COMPONENT_TRANSFORM]->size; i++)
	{
		TransformComponent* tc = arrayGetElementAt(registry.components[COMPONENT_TRANSFORM], i);
		printf("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	}
	return;
	Game game = gameCreate();

	game = gameInit(game);

	game = gameRun(game);

	gameDestroy(game);
	
}