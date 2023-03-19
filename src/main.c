#include <stdio.h>
#include "ECS/ECS.h"
int main(void)
{
	ArrayHeader* array = arrayCreate(10, sizeof(Entity));

	{
		Entity e = entityCreate();

		arrayAddElement(array, &e);
	}
	{
		Entity e = entityCreate();

		arrayAddElement(array, &e);
	}
	{
		Entity e = entityCreate();

		arrayAddElement(array, &e);
	}
	{
		Entity e = entityCreate();

		arrayAddElement(array, &e);
	}
	{
		Entity e = entityCreate();

		arrayAddElement(array, &e);
	}

	return 0;
}