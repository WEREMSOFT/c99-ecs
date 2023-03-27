#include <SDL2/SDL.h>
#include <stdbool.h>
#include "assetStore.h"

#define MAX_ENTITIES 600
const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

enum ComponentEnum
{
	COMPONENT_NONE,
	COMPONENT_TRANSFORM,
	COMPONENT_RIGID_BODY,
	COMPONENT_SPRITE,
	COMPONENT_COUNT
};

enum SystemEnum
{
	SYSTEM_MOVEMENT,
	SYSTEM_RENDER,
	SYSTEM_COUNT
};

#include "ecs/ecs.h"
#include "components.h"

Registry registryCreate()
{
	Registry returnValue = {0};
	returnValue.entities = arrayCreate(MAX_ENTITIES, sizeof(Entity));

	returnValue.componentSignatures = arrayCreate(MAX_ENTITIES, sizeof(Bitset));

	returnValue.components[COMPONENT_TRANSFORM] = arrayCreate(MAX_ENTITIES, sizeof(TransformComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));
	returnValue.components[COMPONENT_SPRITE] = arrayCreate(MAX_ENTITIES, sizeof(SpriteComponent));

	for(int i = 0; i < COMPONENT_COUNT; i ++)
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < SYSTEM_COUNT; i++)
		returnValue.entitiesPerSystem[i] = arrayCreate(MAX_ENTITIES, sizeof(Entity));
	
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_TRANSFORM);
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_RIGID_BODY);

	returnValue.systemInterestSignatures[SYSTEM_RENDER] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_RENDER], COMPONENT_SPRITE);

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

void load_csv(char* filename, int rows, int cols, int matrix[][cols]) {
    FILE* fp;
    char buffer[1024];
    int row = 0, col = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: failed to open file %s\n", filename);
        exit(1);
    }

    while (fgets(buffer, 1024, fp) != NULL && row < rows) {
        char* token = strtok(buffer, ",");
        while (token != NULL && col < cols) {
            matrix[row][col] = atoi(token);
            col++;
            token = strtok(NULL, ",");
        }
        col = 0;
        row++;
    }

    fclose(fp);
}

Game gameInit(Game _this)
{
	_this.assetStore = assetStoreAddTexture(_this.assetStore, _this.renderer, TEXTURE_TILEMAP_IMAGE, "./assets/tilemaps/jungle.png");

	{
		float scale = 2.;
		int cols = 25, rows = 20;
		int tilemap[rows][cols];

		load_csv("./assets/tilemaps/jungle.map", rows, cols, tilemap);

		for(int y = 0; y < rows; y++)
		{
			for(int x = 0; x < cols; x++)
			{
				Entity entity = entityCreate(_this.registry);
				SDL_Rect srcRect = (SDL_Rect){(tilemap[y][x] % 10) * 32, (tilemap[y][x] / 10) * 32, 32, 32};;
				SDL_Rect destRect = (SDL_Rect){ x * 32, y * 32, 32, 32 };
				
				SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TILEMAP_IMAGE, 32, 32, 0, srcRect.x, srcRect.y, scale);
				spriteComponent = *(SpriteComponent*)entityAddComponent(entity, _this.registry, &spriteComponent, COMPONENT_SPRITE);

				Vector2 position = {destRect.x * scale, destRect.y * scale};
				Vector2 scaleV = {scale, scale};

				TransformComponent transformComponent = {position, scaleV, 0};
				entityAddComponent(entity, _this.registry, &transformComponent, COMPONENT_TRANSFORM);
			}
		}
	}
	return _this;
	// Entity 1
	{
		Entity entity = entityCreate(_this.registry);
		entityAddComponent(entity, _this.registry, &((TransformComponent){3, 4}), COMPONENT_TRANSFORM);
		entityAddComponent(entity, _this.registry, &((RigidBodyComponent){10., 20.}), COMPONENT_RIGID_BODY);
	}
	// Entity 2
	{
		Entity entity = entityCreate(_this.registry);
		entityAddComponent(entity, _this.registry, &((TransformComponent){1, 2}), COMPONENT_TRANSFORM);
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

	renderSystem(_this.registry, _this.assetStore, _this.renderer);

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

	movementSystem(_this.registry);

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