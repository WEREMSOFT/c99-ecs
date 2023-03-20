#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"

#define MAX_ENTITIES 10

enum ComponentEnum
{
	COMPONENT_POSITION,
	COMPONENT_COUNT
};

#include "ecs/ecs.h"

typedef struct 
{
	int x, y, z;
} PositionComponent;

Registry registryCreate()
{
	Registry returnValue = {0};
	returnValue.entities = arrayCreate(MAX_ENTITIES, sizeof(Entity));
	returnValue.components[COMPONENT_POSITION] = arrayCreate(MAX_ENTITIES, sizeof(PositionComponent));
	return returnValue;
}


int main()
{
	Registry registry = registryCreate();

	{
		Entity entity = entityCreate();
		arrayAddElement(registry.entities, &entity);
		entityAddComponent(entity, registry, &((PositionComponent){10, 20, 30}), COMPONENT_POSITION);
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d, %d\n", position->x, position->y, position->z);
			position->x = 11;
			position->y = 22;
			position->z = 33;
		}
		{
			PositionComponent* position = entityGetComponent(entity, registry, COMPONENT_POSITION);
			printf("position: %d, %d, %d\n", position->x, position->y, position->z);
		}
	}

	
}