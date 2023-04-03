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
	Game game = gameCreate();

	game = gameInit(game);

	game = gameRun(game);

	gameDestroy(game);
	
}