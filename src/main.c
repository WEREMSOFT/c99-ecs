#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"

#define MAX_ENTITIES 10

enum ComponentEnum
{
	COMPONENT_NONE,
	COMPONENT_POSITION,
	COMPONENT_RIGID_BODY,
	COMPONENT_COUNT
};

enum SystemEnum
{
	SYSTEM_MOVEMENT,
	SYSTEM_COUNT
};

#include "ecs/ecs.h"

typedef struct
{
	float x,y,z,w;
} Vector;

typedef struct 
{
	int x,y;
} PositionComponent;

typedef struct 
{
	Vector Velocity;
} RigidBodyComponent;

Registry registryCreate()
{
	// TODO 5: Create an array for every system that holds the entities of interests and is updated in the registryUpdate function at the end of the game loop
	Registry returnValue = {0};
	returnValue.entities = arrayCreate(MAX_ENTITIES, sizeof(Entity));

	returnValue.componentSignatures = arrayCreate(MAX_ENTITIES, sizeof(Bitset));

	returnValue.components[COMPONENT_POSITION] = arrayCreate(MAX_ENTITIES, sizeof(PositionComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));

	for(int i = 0; i < COMPONENT_COUNT; i ++)
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));

	for(int i = 0; i < SYSTEM_COUNT; i++)
		returnValue.entitiesPerSystem[i] = arrayCreate(MAX_ENTITIES, sizeof(Entity));
	
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_POSITION);
	returnValue.systemInterestSignatures[SYSTEM_MOVEMENT] = bitsetSet(returnValue.systemInterestSignatures[SYSTEM_MOVEMENT], COMPONENT_RIGID_BODY);

	return returnValue;
}

void updatePositionXSystem(Entity _this, Registry registry)
{
	PositionComponent *position = (PositionComponent *)entityGetComponent(_this, registry, COMPONENT_POSITION);
	position->x++;
}

ArrayHeader* systemGetEntities(int systemId, Registry registry)
{
	ArrayHeader* returnValue = registry.entitiesPerSystem[systemId];
	return returnValue;
}

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