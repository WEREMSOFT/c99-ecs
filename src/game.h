#include <SDL2/SDL.h>
#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_SDL2
#include <cimgui_impl.h>

#define __DEBUG_BUILD__

#define myMalloc malloc
#define myFree free
#define myRealloc realloc
#define myCalloc calloc

#include "core/array.h"
#include "core/bitset.h"
#include "core/logger.h"

#include "assetStore.h"
#include "eventBus.h"

#define MAX_ENTITIES 10
const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

typedef enum
{
	COMPONENT_SPRITE,
	COMPONENT_TRANSFORM,
	COMPONENT_CIRCULAR_MOVEMENT,
	COMPONENT_ANIMATION,
	COMPONENT_RIGID_BODY,
	COMPONENT_KEYBOARD_CONTROLLER,
	COMPONENT_PROJECTILE_EMITTER,
	COMPONENT_COUNT,
} ComponentEnum;

typedef enum
{
	SYSTEM_RENDER,
	SYSTEM_CIRCULAR_MOVEMENT,
	SYSTEM_ANIMATION,
	SYSTEM_KEYBOARD,
	SYSTEM_PROJECTILE_EMITTER,
	SYSTEM_MOVEMENT,
	SYSTEM_COUNT,
} SystemEnum;

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
#include "entitiesCreators.h"
Registry registryCreate()
{
	Registry returnValue = {0};

	returnValue.isDirty = true;

	returnValue.entitiesToDelete = arrayCreate(MAX_ENTITIES, sizeof(int));
	returnValue.componentSignatures = arrayCreate(MAX_ENTITIES, sizeof(Bitset));
	returnValue.tags = arrayCreateAndInitToZero(MAX_ENTITIES, sizeof(Bitset));
	returnValue.tags->size = returnValue.tags->capacity;

	returnValue.components[COMPONENT_TRANSFORM] = arrayCreate(MAX_ENTITIES, sizeof(TransformComponent));
	returnValue.components[COMPONENT_SPRITE] = arrayCreate(MAX_ENTITIES, sizeof(SpriteComponent));
	returnValue.components[COMPONENT_CIRCULAR_MOVEMENT] = arrayCreate(MAX_ENTITIES, sizeof(CircularMovementComponent));
	returnValue.components[COMPONENT_ANIMATION] = arrayCreate(MAX_ENTITIES, sizeof(AnimationComponent));
	returnValue.components[COMPONENT_KEYBOARD_CONTROLLER] = arrayCreate(MAX_ENTITIES, sizeof(KeyboardComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));
	returnValue.components[COMPONENT_PROJECTILE_EMITTER] = arrayCreate(MAX_ENTITIES, sizeof(ProjectileEmitterComponent));

	for(int i = 0; i < COMPONENT_COUNT; i ++)
	{
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));
		memset(returnValue.entity2Component[i]->data, -1, returnValue.entity2Component[i]->capacity);
	}

	for(int i = 0; i < GROUP_COUNT; i++)
		returnValue.groups[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < SYSTEM_COUNT; i++)
		returnValue.entitiesPerSystem[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	// BUILD SYSTEM_CIRCULAR_MOVEMENT SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT], COMPONENT_TRANSFORM);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_CIRCULAR_MOVEMENT], COMPONENT_CIRCULAR_MOVEMENT);

	// BUILD SYSTEM_ANIMATION SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_ANIMATION], COMPONENT_SPRITE);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_ANIMATION], COMPONENT_ANIMATION);

	// BUILD SYSTEM_RENDER SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_RENDER], COMPONENT_SPRITE);

	// BUILD SYSTEM_KEYBOARD_CONTROLLER SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_KEYBOARD], COMPONENT_KEYBOARD_CONTROLLER);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_KEYBOARD], COMPONENT_RIGID_BODY);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_KEYBOARD], COMPONENT_TRANSFORM);

	// BUILD MOVEMENT_SYSTEM SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_RIGID_BODY);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_TRANSFORM);

	// BUILD PROJECTILE_EMITTER SIGNATURE
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_PROJECTILE_EMITTER], COMPONENT_TRANSFORM);
	bitsetSet(&returnValue.systemInterestSignatures[SYSTEM_PROJECTILE_EMITTER], COMPONENT_PROJECTILE_EMITTER);
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
	EventBus eventBus;
	ImGuiIO* io;
} Game;

void setupDearImgui(Game* _this)
{
    igCreateContext(NULL);
    _this->io = igGetIO();
	_this->io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	#ifdef IMGUI_HAS_DOCK
	ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	#endif

	ImGui_ImplSDL2_InitForSDLRenderer(_this->window, _this->renderer);
    ImGui_ImplSDLRenderer_Init(_this->renderer);
  	
  	igStyleColorsDark(NULL);
}

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
		0);

	assert(_this.window != NULL && "Error creating SDL window");

 	_this.renderer = SDL_CreateRenderer(_this.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	assert(_this.renderer != NULL && "Error creating renderer");

	_this.eventBus = eventBusCreate();

	setupDearImgui(&_this);

	return _this;
}

void gameLoadAssets(Game *_this)
{
	#define CREATE_TEXTURE_ASSET(textureId, imagePath) assetStoreAddTexture(&_this->assetStore, _this->renderer, (textureId), (imagePath))
	CREATE_TEXTURE_ASSET(TEXTURE_TILE_MAP, "./assets/tilemaps/jungle.png");
	CREATE_TEXTURE_ASSET(TEXTURE_TREE, "./assets/images/tree.png");
	CREATE_TEXTURE_ASSET(TEXTURE_TANK_TIGER_UP, "./assets/images/tank-tiger-up.png");
	CREATE_TEXTURE_ASSET(TEXTURE_TRUCK_FORD_DOWN, "./assets/images/tank-tiger-up.png");
	CREATE_TEXTURE_ASSET(TEXTURE_TRUCK_FORD_DOWN, "./assets/images/tank-tiger-up.png");
	CREATE_TEXTURE_ASSET(TEXTURE_LANDING_BASE, "./assets/images/landing-base.png");
	CREATE_TEXTURE_ASSET(TEXTURE_LANDING_BASE, "./assets/images/landing-base.png");
	CREATE_TEXTURE_ASSET(TEXTURE_CHOPPER, "./assets/images/chopper-spritesheet.png");
	CREATE_TEXTURE_ASSET(TEXTURE_BULLET, "./assets/images/bullet.png");
	CREATE_TEXTURE_ASSET(TEXTURE_INFERNO_FLOOR_1, "./assets/Floor_Lower_1.png");
	CREATE_TEXTURE_ASSET(TEXTURE_INFERNO_FLOOR_2, "./assets/Floor_Lower_2.png");
	CREATE_TEXTURE_ASSET(TEXTURE_INFERNO_COLUMN, "./assets/Column_5.png");
	CREATE_TEXTURE_ASSET(TEXTURE_INFERNO_TILE_MAP, "./assets/tilemaps/floorISO.png");
	#undef CREATE_TEXTURE_ASSET
}

void gameInit(Game* _this)
{
	gameLoadAssets(_this);

	float scale = 1.;
	Vector2 scaleV = {scale, scale};
	tilemapCreateInferno(&_this->registry, scaleV);
	
	float phase = 0.;
	registryAddTree(&_this->registry, 1, 1, scaleV);
	registryAddTree(&_this->registry, 2, 1, scaleV);
	registryAddTree(&_this->registry, 3, 1, scaleV);
	registryAddTree(&_this->registry, 4, 1, scaleV);
	registryAddTree(&_this->registry, 5, 1, scaleV);

	// ADD ANIMATED CHOPPER
	helicopterCreate(&_this->registry, scaleV);
}

void gameDestroy(Game _this)
{
	SDL_DestroyRenderer(_this.renderer);
	SDL_DestroyWindow(_this.window);
	SDL_Quit();
	arrayDestroy(_this.registry.tags);
	arrayDestroy(_this.registry.componentSignatures);
	arrayDestroy(_this.registry.entitiesToDelete);

	for(int i = 0; i < GROUP_COUNT; i++)
	{
		arrayDestroy(_this.registry.groups[i]);
	}

	for(int i = 0; i < COMPONENT_COUNT; i++)
	{
		arrayDestroy(_this.registry.components[i]);
		arrayDestroy(_this.registry.entity2Component[i]);
	}

	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayDestroy(_this.registry.entitiesPerSystem[i]);
	}
}

void gameProcessInput(Game* _this)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		// Handle SDL events
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			_this->isRunning = false;
			break;
		case SDL_KEYDOWN:
			Event evt = {.type = EVENT_TYPE_KEY_DOWN}; 
			KeyboardEventData data = {0};
			data.keyCode = event.key.keysym.sym;
			data.registry = &_this->registry;
			evt.data = &data;

			eventBusEmmitEvent(_this->eventBus, evt);
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				_this->isRunning = false;
			}
			break;
		}
	}
}

void showGUI(Game _this)
{
	ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
	// igShowDemoWindow(&(bool){true});

	igBegin("Debug", &(bool){true}, 0);
	igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _this.io->Framerate, _this.io->Framerate);
	igText("Entities %d", _this.registry.entityCount);
	// igText("Get Memory Usage %d", totalAllocatedMemory);
	igEnd();

	igRender();
	ImDrawData* dd = igGetDrawData();
	ImGui_ImplSDLRenderer_RenderDrawData(dd);
}

void gameRender(Game _this)
{
	SDL_SetRenderDrawColor(_this.renderer, 21, 21, 21, 255);
	SDL_RenderClear(_this.renderer);

	renderSystem(_this.registry, _this.assetStore, _this.renderer);

 	showGUI(_this);
	
	SDL_RenderPresent(_this.renderer);
}

static void delayFrameBasedOnElapsedTime(Game _this)
{
	int currentTicks = SDL_GetTicks(); 
	int timeToWait = MILLISECONDS_PER_FRAME - currentTicks + _this.millisecondsPreviousFrame;
	if (timeToWait > 0 && timeToWait < MILLISECONDS_PER_FRAME)
		SDL_Delay(timeToWait);
}

void gameUpdate(Game* _this)
{
	delayFrameBasedOnElapsedTime(*_this);
	_this->registry.frameCount++;

	float deltaTime = (SDL_GetTicks() - _this->millisecondsPreviousFrame) / 1000.f;
	_this->millisecondsPreviousFrame = SDL_GetTicks();

	keyboardControllerSystemAddListener(_this->eventBus);
	projectileEmitterSystemAddListener(_this->eventBus);

	gameProcessInput(_this);

	circularMovementSystem(_this->registry, deltaTime);
	animationSystem(_this->registry);
	movementSystem(&_this->registry, deltaTime, (Vector2){_this->windowWidth, _this->windowHeight});
	registryUpdate(&_this->registry);
	eventBusCleanEventListeners(_this->eventBus);
}

void gameRun(Game* _this)
{
	while (_this->isRunning)
	{
		gameUpdate(_this);
		gameRender(*_this);
	}
}