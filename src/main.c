#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"
#include "core/logger.h"

#include <SDL2/SDL.h>
#include "game.h"

#include "core/logger.h"

#define ADD_ENTITY(x, y) \
	{\
		int entityId = entityCreate(&registry);\
		SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TREE, (x), (y), 0, 0, 0, 2.);\
		entityAddComponent(entityId, &registry, &spriteComponent, COMPONENT_SPRITE);\
		\
		TransformComponent transformComponent = {{(x), (y)}, scaleV, 0};\
		entityAddComponent(entityId, &registry, &transformComponent, COMPONENT_TRANSFORM);\
		\
		CircularMovementComponent cmc = {.phase = 0., .center = {(x) * 100., (y) * 100.}, .radius = 100. };\
		\
		entityAddComponent(entityId, &registry, &cmc, COMPONENT_CIRCULAR_MOVEMENT);\
	}

int main()
{
	float scale = 1.;
	Vector2 scaleV = {scale, scale};
	Registry registry = registryCreate();

	ADD_ENTITY(10, 15);
	ADD_ENTITY(20, 25);
	ADD_ENTITY(30, 35);
	ADD_ENTITY(40, 45);
	ADD_ENTITY(50, 55);
	ADD_ENTITY(60, 65);
	ADD_ENTITY(70, 75);

	registry = registryUpdate(registry);
	// entityDelete(0, &registry);
	// {
	// 	ArrayHeader* entities = systemGetEntities(SYSTEM_RENDER, registry);
	// 	for(int i = 0; i < entities->size; i++)
	// 	{
	// 		SpriteComponent* tc = entityGetComponent(i, registry, COMPONENT_SPRITE);
	// 		loggerLog("%d, %d, %d", i, tc->width, tc->height);
	// 	}
	// }

	{
		loggerLog("entity count: %d", registry.entityCount);
		loggerLog("sprite components count: %d", registry.components[COMPONENT_SPRITE]->size);
		loggerLog("map size: %d", registry.entity2Component[COMPONENT_SPRITE]->size);
		ArrayHeader* sc = systemGetEntities(SYSTEM_RENDER, registry);
		loggerLog("entities for the render component: %d", sc->size);
	}
	loggerError("##########################");
	entityDelete(2, &registry);
	{
		loggerLog("entity count: %d", registry.entityCount);
		loggerLog("sprite components count: %d", registry.components[COMPONENT_SPRITE]->size);
		loggerLog("map size: %d", registry.entity2Component[COMPONENT_SPRITE]->size);
		ArrayHeader* sc = systemGetEntities(SYSTEM_RENDER, registry);
		loggerLog("entities for the render component: %d", sc->size);
	}
	// {
	// 	ArrayHeader* entities = systemGetEntities(SYSTEM_RENDER, registry);
	// 	for(int i = 0; i < entities->size; i++)
	// 	{
	// 		SpriteComponent* tc = entityGetComponent(i, registry, COMPONENT_SPRITE);
	// 		loggerWarning("%d, %d, %d", i, tc->width, tc->height);
	// 	}
	// }

	// printf("#########\n");
	// loggerWarning("updating registry\n");
	// registry = registryUpdate(registry);
	// for(int i = 0; i < registry.components[COMPONENT_TRANSFORM]->size; i++)
	// {
	// 	TransformComponent* tc = arrayGetElementAt(registry.components[COMPONENT_TRANSFORM], i);
	// 	loggerError("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	// }
	// printf("#########\n");

	// // DELETING ENTITY
	// loggerWarning("deleting entity\n");
	// entityDelete(0, &registry);
	// for(int i = 0; i < registry.components[COMPONENT_TRANSFORM]->size; i++)
	// {
	// 	TransformComponent* tc = arrayGetElementAt(registry.components[COMPONENT_TRANSFORM], i);
	// 	loggerLog("%d, %f, %f\n", i, tc->position.x, tc->position.y);
	// }
	return 0;
	Game game = gameCreate();

	game = gameInit(game);

	game = gameRun(game);

	gameDestroy(game);
	
}