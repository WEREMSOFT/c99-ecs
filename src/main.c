#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"

#define MAX_ENTITIES 10

#include "ecs/ecs.h"

int main()
{
	ArrayHeader* entities = (ArrayHeader*)arrayCreate(10, sizeof(Entity));

	{
		Entity e = {1};
		arrayInsertElement(entities, &e);
		printf("%d\n", e.Id);
	}
	{
		Entity e = {2};
		arrayInsertElement(entities, &e);
		printf("%d\n", e.Id);
	}
	{
		Entity e = {3};
		arrayInsertElement(entities, &e);
		printf("%d\n", e.Id);
	}

	printf("PRINTING ARRAY");
	for(int i = 0; i < entities->size; i++)
	{
		int id = entities->data[i];
		printf("%d\n", id);
	}
}