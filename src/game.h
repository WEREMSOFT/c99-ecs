#include <SDL2/SDL.h>
#include <stdbool.h>
#include "assetStore.h"

#define MAX_ENTITIES 10
const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

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
	SDL_Window *window;
	SDL_Renderer *renderer;
	Vector windowSize;
	int windowWidth;
	int windowHeight;
	int millisecondsPreviousFrame;
	bool isRunning;
	AssetStore assetStore;
} Game;

Game gameCreate()
{
	Game _this = {0};
	_this.windowWidth = 800;
	_this.windowHeight = 600;
	_this.registry = registryCreate();
	_this.isRunning = true;

	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0 && "Error initializing SDL\n");

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	_this.windowSize.x = 800;
	_this.windowSize.y = 600;

	_this.window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_this.windowWidth,
		_this.windowHeight,
		SDL_WINDOW_BORDERLESS);

	assert(_this.window != NULL && "Error creating SDL window");

 	_this.renderer = SDL_CreateRenderer(_this.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	assert(_this.renderer != NULL && "Error creating renderer");

	return _this;
}

Game gameInit(Game _this)
{
	_this.assetStore = assetStoreAddTexture(_this.assetStore, _this.renderer, TEXTURE_TILEMAP_IMAGE, "./assets/tilemaps/jungle.png");
	// Entity 1
	{
		Entity entity = entityCreate(_this.registry);
		entityAddComponent(entity, _this.registry, &((PositionComponent){3, 4}), COMPONENT_POSITION);
		entityAddComponent(entity, _this.registry, &((RigidBodyComponent){10., 20.}), COMPONENT_RIGID_BODY);
	}
	// Entity 2
	{
		Entity entity = entityCreate(_this.registry);
		entityAddComponent(entity, _this.registry, &((PositionComponent){1, 2}), COMPONENT_POSITION);
		entityAddComponent(entity, _this.registry, &((RigidBodyComponent){30., 40.}), COMPONENT_RIGID_BODY);
	}
	return _this;
}

void gameDestroy(Game _this)
{
	SDL_DestroyRenderer(_this.renderer);
	SDL_DestroyWindow(_this.window);
	SDL_Quit();
}

Game gameProcessInput(Game _this)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		// Handle SDL events
		switch (event.type)
		{
		case SDL_QUIT:
			_this.isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				_this.isRunning = false;
			}
			break;
		}
	}
	return _this;
}

void gameRender(Game _this)
{
	SDL_SetRenderDrawColor(_this.renderer, 21, 21, 21, 255);
	SDL_RenderClear(_this.renderer);
	
	SDL_Texture* texture = assetStoreGetTexture(_this.assetStore, TEXTURE_TILEMAP_IMAGE);

	SDL_Rect srcRect = {0, 0, 100, 100};

	SDL_RenderCopyEx(_this.renderer, texture, &srcRect, &srcRect, 0, NULL, false);

	SDL_RenderPresent(_this.renderer);
}

Game gameUpdate(Game _this)
{
	int currentTicks = SDL_GetTicks();
	int timeToWait = MILLISECONDS_PER_FRAME - currentTicks + _this.millisecondsPreviousFrame;
	if (timeToWait > 0 && timeToWait < MILLISECONDS_PER_FRAME)
		SDL_Delay(timeToWait);

	float deltaTime = (SDL_GetTicks() - _this.millisecondsPreviousFrame) / 1000.f;
	_this.millisecondsPreviousFrame = SDL_GetTicks();

	printf("########################################\n");
	for(int i = 0; i < _this.registry.entities->size; i++)
	{
		Entity entity = *(Entity *)arrayGetElementAt(_this.registry.entities, i);
		{
			PositionComponent* position = entityGetComponent(entity, _this.registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
			RigidBodyComponent* rigidBody = entityGetComponent(entity, _this.registry, COMPONENT_RIGID_BODY);
			printf("velocity: %f, %f\n", rigidBody->Velocity.x, rigidBody->Velocity.y);
		}
	}

	movementSystem(_this.registry);

	printf("########################################\n");
	for(int i = 0; i < _this.registry.entities->size; i++)
	{
		Entity entity = *(Entity *)arrayGetElementAt(_this.registry.entities, i);
		{
			PositionComponent* position = entityGetComponent(entity, _this.registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
			RigidBodyComponent* rigidBody = entityGetComponent(entity, _this.registry, COMPONENT_RIGID_BODY);
			printf("velocity: %f, %f\n", rigidBody->Velocity.x, rigidBody->Velocity.y);
		}
	}
	registryUpdate(_this.registry);
	return _this;
}

Game gameRun(Game _this)
{
	while (_this.isRunning)
	{
		_this = gameProcessInput(_this);
		_this = gameUpdate(_this);
		gameRender(_this);
	}
	return _this;
}