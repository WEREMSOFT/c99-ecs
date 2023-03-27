void movementSystem(Registry registry)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_MOVEMENT,  registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		Entity* entity = arrayGetElementAt(entities, entityPerSystemIndex);
		RigidBodyComponent* rigidBody = entityGetComponent(*entity, registry, COMPONENT_RIGID_BODY);
		TransformComponent* transform = entityGetComponent(*entity, registry, COMPONENT_TRANSFORM);
		transform->position.y += rigidBody->Velocity.y;
		transform->position.x += rigidBody->Velocity.x;
	}
}

void renderSystem(Registry registry, AssetStore assetStore, SDL_Renderer* renderer)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_RENDER, registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		Entity* entity = arrayGetElementAt(entities, entityPerSystemIndex);
		SpriteComponent* sprite = entityGetComponent(*entity, registry, COMPONENT_SPRITE);
		TransformComponent* transform = entityGetComponent(*entity, registry, COMPONENT_TRANSFORM);

		SDL_Texture *texture = assetStoreGetTexture(assetStore, sprite->assetId);

		SDL_Rect destRect = {
				transform->position.x,
				transform->position.y,
				sprite->width * transform->scale.x,
				sprite->height * transform->scale.y};

		SDL_RenderCopyEx(renderer, texture, &sprite->srcRect, &destRect, transform->rotation, NULL, sprite->flip);
	}
}