#include <SDL2/SDL.h>
#include <stdbool.h>
#include "assetStore.h"

#define MAX_ENTITIES 600
const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

enum ComponentEnum
{
	COMPONENT_NONE,
	COMPONENT_CIRCULAR_MOVEMENT,
	COMPONENT_TRANSFORM,
	COMPONENT_RIGID_BODY,
	COMPONENT_SPRITE,
	COMPONENT_COUNT
};

enum SystemEnum
{
	SYSTEM_MOVEMENT,
	SYSTEM_CIRCULAR_MOVEMENT,
	SYSTEM_RENDER,
	SYSTEM_COUNT
};

enum TagsEnum
{
	TAG_PLAYER,
	TAG_ENEMY,
	TAG_TILE,
	TAG_COUNT
};

enum GroupsEnum
{
	GROUP_TILES,
	GROUP_ENEMIES,
	GROUP_COUNT
};

#include "ecs/ecs.h"
#include "components.h"

Registry registryCreate()
{
	Registry returnValue = {0};

	returnValue.componentSignatures = arrayCreate(MAX_ENTITIES, sizeof(Bitset));
	returnValue.tags = arrayCreateAndInitToZero(MAX_ENTITIES, sizeof(Bitset));
	returnValue.tags->size = returnValue.tags->capacity;

	returnValue.components[COMPONENT_TRANSFORM] = arrayCreate(MAX_ENTITIES, sizeof(TransformComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));
	returnValue.components[COMPONENT_SPRITE] = arrayCreate(MAX_ENTITIES, sizeof(SpriteComponent));
	returnValue.components[COMPONENT_CIRCULAR_MOVEMENT] = arrayCreate(MAX_ENTITIES, sizeof(CircularMovementComponent));

	for(int i = 0; i < COMPONENT_COUNT; i ++)
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < GROUP_COUNT; i++)
		returnValue.groups[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < SYSTEM_COUNT; i++)
		returnValue.entitiesPerSystem[i] = arrayCreate(MAX_ENTITIES, sizeof(int));
	
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_TRANSFORM);
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_RIGID_BODY);


	returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT], COMPONENT_TRANSFORM);
	returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT], COMPONENT_CIRCULAR_MOVEMENT);

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

static void loadCSV(char* filename, int rows, int cols, int matrix[][cols]) {
    FILE* fp;
    char buffer[1024];
    int row = 0, col = 0;

    fp = fopen(filename, "r");

	assert(fp != NULL && "Error: failed to open file");

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
	_this.assetStore = assetStoreAddTexture(_this.assetStore, _this.renderer, TEXTURE_TREE_IMAGE, "./assets/images/tree.png");

		float scale = 1.;
		Vector2 scaleV = {scale, scale};

	{
		int cols = 25, rows = 20;
		int tilemap[rows][cols];

		loadCSV("./assets/tilemaps/jungle.map", rows, cols, tilemap);

		for(int y = 0; y < rows; y++)
		{
			for(int x = 0; x < cols; x++)
			{
				int entityId = entityCreate(&_this.registry);
				entityAddTag(entityId, _this.registry, TAG_TILE);

				SDL_Rect srcRect = (SDL_Rect){(tilemap[y][x] % 10) * 32, (tilemap[y][x] / 10) * 32, 32, 32};;
				SDL_Rect destRect = (SDL_Rect){ x * 32, y * 32, 32, 32 };
				
				SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TILEMAP_IMAGE, 32, 32, 0, srcRect.x, srcRect.y, scale);
				entityAddComponent(entityId, &_this.registry, &spriteComponent, COMPONENT_SPRITE);

				Vector2 position = {destRect.x * scale, destRect.y * scale};

				TransformComponent transformComponent = {position, scaleV, 0};
				entityAddComponent(entityId, &_this.registry, &transformComponent, COMPONENT_TRANSFORM);
			}
		}
	}
	// Entity 1
	{
		int entityId = entityCreate(&_this.registry);

		SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TREE_IMAGE, 16, 32, 0, 0, 0, 2.);
		entityAddComponent(entityId, &_this.registry, &spriteComponent, COMPONENT_SPRITE);
		
		TransformComponent transformComponent = {{10, 10}, scaleV, 0};
		entityAddComponent(entityId, &_this.registry, &transformComponent, COMPONENT_TRANSFORM);

		CircularMovementComponent cmc = {.phase = 0., .center = {100., 100.}, .radius = 100. };

		entityAddComponent(entityId, &_this.registry, &cmc, COMPONENT_CIRCULAR_MOVEMENT);

		// entityAddComponent(entityId, &_this.registry, &((RigidBodyComponent){0., 0.}), COMPONENT_RIGID_BODY);
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
	circularMovementSystem(_this.registry);

	_this.registry = registryUpdate(_this.registry);
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