#include "assetStore.h"

typedef struct
{
	float x,y,z,w;
} Vector;

typedef struct
{
	float x,y;
} Vector2;


// POSITION
typedef struct 
{
	Vector2 position;
	Vector2 scale;
	double rotation;
} TransformComponent;

// RIGIDBODY
typedef struct 
{
	Vector Velocity;
} RigidBodyComponent;

// CIRCULAR MOVEMENT
typedef struct
{
	float phase;
	float radius;
	Vector center;
} CircularMovementComponent;


// SPRITE
typedef struct
{
int width;
	int height;
	int zIndex;
	bool isFixed;
	TextureIdEnum assetId;
	SDL_Rect srcRect;
	SDL_RendererFlip flip;
} SpriteComponent;

SpriteComponent spriteComponentCreate(
	const TextureIdEnum assetId, 
	int with, int height, 
	int zIndex, int srcRectX, int srcRectY, 
	bool isFixed)
{
	SpriteComponent _this = {0};
	_this.zIndex = zIndex;
	_this.width = with;
	_this.height = height;
	_this.assetId = assetId;
	_this.srcRect = (SDL_Rect){srcRectX, srcRectY, with, height};
	_this.isFixed = isFixed;
	_this.flip = SDL_FLIP_NONE;

	return _this;
}