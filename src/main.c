#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"

#include <SDL2/SDL.h>
#include <game.h>

int main()
{
	Registry registry = registryCreate();
	// Entity 1
	{
		Entity entity = entityCreate(registry);
		entityAddComponent(entity, registry, &((PositionComponent){3, 4}), COMPONENT_POSITION);
		entityAddComponent(entity, registry, &((RigidBodyComponent){10., 20.}), COMPONENT_RIGID_BODY);
	}
	// Entity 2
	{
		Entity entity = entityCreate(registry);
		entityAddComponent(entity, registry, &((PositionComponent){1, 2}), COMPONENT_POSITION);
		entityAddComponent(entity, registry, &((RigidBodyComponent){30., 40.}), COMPONENT_RIGID_BODY);
	}

	registryUpdate(registry);

	printf("########################################\n");
	for(int i = 0; i < registry.entities->size; i++)
	{
		Entity entity = *(Entity *)arrayGetElementAt(registry.entities, i);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
			RigidBodyComponent* rigidBody = entityGetComponent(entity, registry, COMPONENT_RIGID_BODY);
			printf("velocity: %f, %f\n", rigidBody->Velocity.x, rigidBody->Velocity.y);
		}
	}

	movementSystem(registry);

	printf("########################################\n");
	for(int i = 0; i < registry.entities->size; i++)
	{
		Entity entity = *(Entity *)arrayGetElementAt(registry.entities, i);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
			RigidBodyComponent* rigidBody = entityGetComponent(entity, registry, COMPONENT_RIGID_BODY);
			printf("velocity: %f, %f\n", rigidBody->Velocity.x, rigidBody->Velocity.y);
		}
	}
}