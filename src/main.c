#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"

#define MAX_ENTITIES 10

enum ComponentEnum
{
	COMPONENT_POSITION,
	COMPONENT_RIGID_BODY,
	COMPONENT_COUNT
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
	Registry returnValue = {0};
	returnValue.entities = arrayCreate(MAX_ENTITIES, sizeof(Entity));

	returnValue.components[COMPONENT_POSITION] = arrayCreate(MAX_ENTITIES, sizeof(PositionComponent));
	returnValue.components[COMPONENT_RIGID_BODY] = arrayCreate(MAX_ENTITIES, sizeof(RigidBodyComponent));

	return returnValue;
}

int main()
{
	Registry registry = registryCreate();
	// Entity 1
	{
		Entity entity = entityCreate(registry);
		entityAddComponent(entity, registry, &((PositionComponent){10, 20}), COMPONENT_POSITION);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d \n", position->x, position->y);
			position->x = 11;
			position->y = 22;
		}
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
		}
	}
	// Entity 2
	{
		Entity entity = entityCreate(registry);
		entityAddComponent(entity, registry, &((RigidBodyComponent){10., 20.}), COMPONENT_RIGID_BODY);
		entityAddComponent(entity, registry, &((PositionComponent){100, 200}), COMPONENT_POSITION);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d \n", position->x, position->y);
			position->x = 110;
			position->y = 220;
		}
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
		}
	}
	
}