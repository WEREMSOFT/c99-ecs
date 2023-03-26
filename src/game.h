#include <SDL2/SDL.h>


#define MAX_ENTITIES 10

enum ComponentEnum
{
	COMPONENT_NONE,
	COMPONENT_POSITION,
	COMPONENT_RIGID_BODY,
	COMPONENT_COUNT
};

enum SystemEnum
{
	SYSTEM_MOVEMENT,
	SYSTEM_COUNT
};

#include "ecs/ecs.h"
#include "components.h"

Registry registryCreate()
{
	Registry returnValue = {0};
	returnValue.entities = arrayCreate(MAX_ENTITIES, sizeof(Entity));

	returnValue.componentSignatures = arrayCreate(MAX_ENTITIES, sizeof(Bitset));

	returnValue.components[COMPONENT_POSITION] = arrayCreate(MAX_ENTITIES, sizeof(PositionComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));

	for(int i = 0; i < COMPONENT_COUNT; i ++)
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < SYSTEM_COUNT; i++)
		returnValue.entitiesPerSystem[i] = arrayCreate(MAX_ENTITIES, sizeof(Entity));
	
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_POSITION);
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_RIGID_BODY);

	return returnValue;
}

#include "systems.h"

typedef struct
{
	Registry registry;
	Vector windowSize;
} Game;

Game gameCreate()
{
	Game returnValue = {0};
	return returnValue;
}

void gameInit(Game _this)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::Err("Error initializing SDL");
		return;
	}

	if(TTF_Init() != 0)
	{
		Logger::Err("Error initializing SDL TTF.");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	_this.windowSize.x = 800;	// displayMode.w;
	_this.windowSize.y = 600; // displayMode.h;

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		Logger::Err("Error creating window");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		Logger::Err("Error creating renderer");
		return;
	}

}