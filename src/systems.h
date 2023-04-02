#include <math.h>

void movementSystem(Registry registry)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_MOVEMENT,  registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		int* entityId = arrayGetElementAt(entities, entityPerSystemIndex);
		RigidBodyComponent* rigidBody = entityGetComponent(*entityId, registry, COMPONENT_RIGID_BODY);
		TransformComponent* transform = entityGetComponent(*entityId, registry, COMPONENT_TRANSFORM);
		transform->position.y += rigidBody->Velocity.y;
		transform->position.x += rigidBody->Velocity.x;
	}
}

void circularMovementSystem(Registry registry)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_CIRCULAR_MOVEMENT,  registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		int* entityId = arrayGetElementAt(entities, entityPerSystemIndex);
		TransformComponent* transform = entityGetComponent(*entityId, registry, COMPONENT_TRANSFORM);
		CircularMovementComponent* cmc = entityGetComponent(*entityId, registry, COMPONENT_CIRCULAR_MOVEMENT);

		transform->position.y = cmc->center.y + sinf(cmc->phase) * cmc->radius;
		transform->position.x = cmc->center.x + cosf(cmc->phase) * cmc->radius;

		cmc->phase += 0.2;
	}
}

void renderSystem(Registry registry, AssetStore assetStore, SDL_Renderer* renderer)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_RENDER, registry);
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