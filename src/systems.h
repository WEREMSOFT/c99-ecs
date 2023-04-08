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

void renderSystem(Registry registry, AssetStore assetStore, SDL_Renderer* renderer)
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
				transform->position.x,
				transform->position.y,
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

#endif