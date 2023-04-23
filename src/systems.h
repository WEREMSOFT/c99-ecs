#ifndef __SYSTEMS_H__
#define __SYSTEMS_H__

#include <math.h>
#include <SDL2/SDL.h>

void circularMovementSystem(Registry registry, float deltaTime)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_CIRCULAR_MOVEMENT,  registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		int entityId = arrayGetElementAtI(entities, entityPerSystemIndex);
		TransformComponent* transform = entityGetComponent(entityId, registry, COMPONENT_TRANSFORM);
		CircularMovementComponent* cmc = entityGetComponent(entityId, registry, COMPONENT_CIRCULAR_MOVEMENT);

		transform->position.y = cmc->center.y + sinf(cmc->phase) * cmc->radius;
		transform->position.x = cmc->center.x + cosf(cmc->phase) * cmc->radius;

		cmc->phase += 5. * deltaTime;
	}
}

void renderSystem(Registry registry, AssetStore assetStore, SDL_Renderer* renderer, SDL_Rect camera)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_RENDER, registry);
	// Sort entities by zindex
	{
		int comp(const int* elm1, const int* elm2)
		{
			SpriteComponent* sprite1 = entityGetComponent(*elm1, registry, COMPONENT_SPRITE);
			SpriteComponent* sprite2 = entityGetComponent(*elm2, registry, COMPONENT_SPRITE);

			return sprite1->zIndex - sprite2->zIndex;
		}

		qsort(entities->data, entities->size, entities->dataTypeSize, comp);
	}

	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		int* entityId = arrayGetElementAt(entities, entityPerSystemIndex);
		SpriteComponent* sprite = entityGetComponent(*entityId, registry, COMPONENT_SPRITE);
		TransformComponent* transform = entityGetComponent(*entityId, registry, COMPONENT_TRANSFORM);

		SDL_Texture *texture = assetStoreGetTexture(assetStore, sprite->assetId);

		SDL_Rect destRect = {
				transform->position.x - camera.x,
				transform->position.y - camera.y,
				sprite->width * transform->scale.x,
				sprite->height * transform->scale.y};

		SDL_RenderCopyEx(renderer, texture, &sprite->srcRect, &destRect, transform->rotation, NULL, sprite->flip);
	}
}

// ANIMATION
void animationSystem(Registry registry)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_ANIMATION, registry);
	for(int i = 0; i < entities->size; i++)
	{
		int entityId = arrayGetElementAtI(entities, i);
		AnimationComponent* animation = entityGetComponent(entityId, registry, COMPONENT_ANIMATION);
		SpriteComponent* sprite = entityGetComponent(entityId, registry, COMPONENT_SPRITE);

		animation->currentFrame = ((SDL_GetTicks() - animation->startTime) * animation->frameRateSpeed / 1000) % animation->numFrames;
		sprite->srcRect.x = animation->currentFrame * sprite->width;
	}
}

// KEYBOARD CONTROLLER
typedef struct 
{
	Registry* registry;
	SDL_KeyCode keyCode;
} KeyboardEventData;

void keyboardHandler(Event event)
{
	KeyboardEventData data = *(KeyboardEventData*)event.data;
	ArrayHeader* entities = systemGetEntities(SYSTEM_KEYBOARD, *data.registry);
	for(int i = 0; i < entities->size; i++)
	{
		int entityId = arrayGetElementAtI(entities, i);
		RigidBodyComponent* rigidBody = entityGetComponent(entityId, *data.registry, COMPONENT_RIGID_BODY);
		KeyboardComponent* keyboardComponent = entityGetComponent(entityId, *data.registry, COMPONENT_KEYBOARD_CONTROLLER);
		SpriteComponent* sprite = entityGetComponent(entityId, *data.registry, COMPONENT_SPRITE);
				
		switch(data.keyCode)
		{
			case SDLK_UP:
				rigidBody->velocity.x = 0;
				rigidBody->velocity.y = -keyboardComponent->velocity;
				sprite->srcRect.y = sprite->height * 0;
				break;
			case SDLK_DOWN:
				rigidBody->velocity.x = 0;
				rigidBody->velocity.y = keyboardComponent->velocity;
				sprite->srcRect.y = sprite->height * 2;
				break;
			case SDLK_RIGHT:
				rigidBody->velocity.y = 0;
				rigidBody->velocity.x = keyboardComponent->velocity;
				sprite->srcRect.y = sprite->height * 1;
				break;
			case SDLK_LEFT:
				rigidBody->velocity.y = 0;
				rigidBody->velocity.x = -keyboardComponent->velocity;
				sprite->srcRect.y = sprite->height * 3;
				break;
		}
	}
}

void keyboardControllerSystemAddListener(EventBus eventBus)
{
	eventBusAddEventListener(eventBus, EVENT_TYPE_KEY_DOWN, keyboardHandler);
}

// MOVEMENT SYSTEM
void movementSystem(Registry* registry, float deltaTime, Vector2 screenSize, Vector2 mapSize)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_MOVEMENT, *registry);
	for(int i = 0; i < entities->size; i++)
	{

		int entityId = arrayGetElementAtI(entities, i);
		TransformComponent* transform = entityGetComponent(entityId, *registry, COMPONENT_TRANSFORM);

		if(entityHasTag(entityId, registry, TAG_PLAYER))
		{
			transform->position.x = min(max(transform->position.x, 0), mapSize.x);
			transform->position.y = min(max(transform->position.y, 0), mapSize.y);
		}

		if(transform->position.x <0 || transform->position.x > mapSize.x || transform->position.y < 0 || transform->position.y > mapSize.y)
		{
			entityQueueForDeletion(entityId, registry);
			continue;
		} 

		RigidBodyComponent* rigidBody = entityGetComponent(entityId, *registry, COMPONENT_RIGID_BODY);

		transform->position.x += rigidBody->velocity.x * deltaTime;
		transform->position.y += rigidBody->velocity.y * deltaTime;
	}
}

// PROJECTILE EMMITER
void projectileEmitterEventListener(Event event)
{
	KeyboardEventData data = *(KeyboardEventData*)event.data;
	if(data.keyCode != SDLK_SPACE) return;
	ArrayHeader* entities = systemGetEntities(SYSTEM_PROJECTILE_EMITTER, *data.registry);

	for(int i = 0; i < entities->size; i++)
	{
		int entityId = arrayGetElementAtI(entities, i);
		TransformComponent* transform = entityGetComponent(entityId, *data.registry, COMPONENT_TRANSFORM);
		RigidBodyComponent* rigidBody = entityGetComponent(entityId, *data.registry, COMPONENT_RIGID_BODY);

		Vector2 newVelocity = rigidBody->velocity;
		newVelocity.x *= 5.;
		newVelocity.y *= 5.;

		registryAddBullet(data.registry, transform->position.x + 16, transform->position.y + 16, (Vector2){1., 1.}, newVelocity);
	}
}

void projectileEmitterSystemAddListener(EventBus eventBus)
{
	eventBusAddEventListener(eventBus, EVENT_TYPE_KEY_DOWN, projectileEmitterEventListener);
}

// CAMERA FOLLOW
void cameraFollowSystem(Game* game)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_CAMERA_FOLLOW, game->registry);
	for(int i = 0; i < entities->size; i++)
	{

		int entityId = arrayGetElementAtI(entities, i);
		TransformComponent transform = *(TransformComponent *)entityGetComponent(entityId, game->registry, COMPONENT_TRANSFORM);

		if (transform.position.x + game->camera.w / 2 < game->mapSize.x)
		{
			game->camera.x = transform.position.x - game->windowWidth / 2;
		}

		if (transform.position.y + game->camera.h / 2 < game->mapSize.y)
		{
			game->camera.y = transform.position.y - game->windowHeight / 2;
		}

		game->camera.x = game->camera.x < 0 ? 0 : game->camera.x;
		game->camera.y = game->camera.y < 0 ? 0 : game->camera.y;

		game->camera.x = (game->camera.x + game->camera.w > game->mapSize.x) ? game->mapSize.x - game->camera.w : game->camera.x;
		game->camera.y = (game->camera.y + game->camera.h > game->mapSize.y) ? game->mapSize.y - game->camera.h : game->camera.y;
	}
}
#endif