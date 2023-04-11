#include "assetStore.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

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
	Vector velocity;
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

// ANIMATION

typedef struct 
{
	int numFrames;
	int currentFrame;
	int frameRateSpeed;
	bool isLoop;
	int startTime;
} AnimationComponent;

AnimationComponent animationComponentCreate(int numFrames, int frameRateSpeed, bool isLoop)
{
	AnimationComponent returnValue = {0};

	returnValue.numFrames = numFrames;
	returnValue.currentFrame = 1;
	returnValue.frameRateSpeed = frameRateSpeed;
	returnValue.isLoop = isLoop;
	returnValue.startTime = SDL_GetTicks();

	return returnValue;
}

// KEYBOARD COMPONENT
typedef struct
{
	float velocity;
} KeyboardComponent;

// PROJECTILES
typedef struct 
{
	bool isFriendly;
	int hitPercentDamage;
	int duration;
	int startTime;
} ProjectileComponent;

ProjectileComponent projectileComponentCreate(bool isFriendly, int hitPercentDamage, int duration)
{
	ProjectileComponent _this;
	_this.isFriendly = isFriendly;
	_this.hitPercentDamage = hitPercentDamage;
	_this.duration = duration;
	_this.startTime = SDL_GetTicks();
	return _this;
}

// PROJECTILE EMITTER
typedef struct
{

	Vector2 projectileVelocity;
	int repeatFrequencyMs;
	int projectileDurationMs;
	int hitPercentDamage;
	bool isFriendly;
	int lastEmissionTimeMs;
} ProjectileEmitterComponent;

ProjectileEmitterComponent projectileEmitterCreate(
		Vector2 projectileVelocity, 
		int repeatFrequencyMs, 
		int projectileDurationMs, 
		int hitPercentDamage, 
		bool isFriendly
		)
{
	ProjectileEmitterComponent _this;
	_this.projectileVelocity = projectileVelocity;
	_this.repeatFrequencyMs = repeatFrequencyMs;
	_this.projectileDurationMs = projectileDurationMs;
	_this.hitPercentDamage = hitPercentDamage;
	_this.isFriendly = isFriendly;
	_this.lastEmissionTimeMs = SDL_GetTicks();
	return _this;
}