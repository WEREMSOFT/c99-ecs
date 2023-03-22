#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"

#define MAX_ENTITIES 10

enum ComponentEnum
{
	COMPONENT_NONE,
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

	for(int i = 0; i < COMPONENT_COUNT; i ++)
	{
		returnValue.entity2Component[i] = arrayCreate(MAX_ENTITIES, sizeof(int));
	}

	return returnValue;
}

void updatePositionXSystem(Entity _this, Registry registry)
{
	PositionComponent *position = (PositionComponent *)entityGetComponent(_this, registry, COMPONENT_POSITION);
	position->x++;
}

void updateVelocityYSystem(Entity _this, Registry registry)
{
	RigidBodyComponent* rigidBody = (RigidBodyComponent*)entityGetComponent(_this, registry, COMPONENT_RIGID_BODY);
	rigidBody->Velocity.y += 10.;
}

int main()
{
	Registry registry = registryCreate();
	// Entity 1
	{
		Entity entity = entityCreate(registry);
		PositionComponent* positionNew = entityAddComponent(entity, registry, &((PositionComponent){10, 20}), COMPONENT_POSITION);
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
		entityAddComponent(entity, registry, &((PositionComponent){100, 200}), COMPONENT_POSITION);
		entityAddComponent(entity, registry, &((RigidBodyComponent){10., 20.}), COMPONENT_RIGID_BODY);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d \n", position->x, position->y);
		}
		updatePositionXSystem(entity, registry);
		updatePositionXSystem(entity, registry);
		updatePositionXSystem(entity, registry);
		updateVelocityYSystem(entity, registry);
		updateVelocityYSystem(entity, registry);
		updateVelocityYSystem(entity, registry);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d\n", position->x, position->y);
			RigidBodyComponent* rigidBody = entityGetComponent(entity, registry, COMPONENT_RIGID_BODY);
			printf("velocity: %f, %f\n", rigidBody->Velocity.x, rigidBody->Velocity.y);
		}

	}



}