void movementSystem(Registry registry)
{
	ArrayHeader* entities = systemGetEntities(SYSTEM_MOVEMENT,  registry);
	for(int entityPerSystemIndex = 0; entityPerSystemIndex < entities->size; entityPerSystemIndex++)
	{
		Entity* entity = arrayGetElementAt(entities, entityPerSystemIndex);
		RigidBodyComponent* rigidBody = (RigidBodyComponent*)entityGetComponent(*entity, registry, COMPONENT_RIGID_BODY);
		PositionComponent* position = (PositionComponent*)entityGetComponent(*entity, registry, COMPONENT_POSITION);
		position->y += rigidBody->Velocity.y;
		position->x += rigidBody->Velocity.x;
	}
}