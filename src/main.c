#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"

#include <SDL2/SDL.h>
#include "game.h"

int main()
{
	// ArrayHeader *array = arrayCreateAndInitToZero(2, sizeof(int));

	// printf("size : %d # capacity: %d\n", array->size, array->capacity);

	// for(int i = 0; i < 10; i++)
	// {
	// 	array = arrayAddElement(array, &i);
	// 	printf("size : %d # capacity: %d\n", array->size, array->capacity);
	// }

	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);

	// for(int i = 0; i < array->size; i++)
	// {
	// 	printf("value : %d\n", *(int*)arrayGetElementAt(array, i));
	// }

	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);
	// arrayDeleteElement(array, 1);

	// for(int i = 0; i < array->size; i++)
	// {
	// 	printf("value : %d\n", *(int*)arrayGetElementAt(array, i));
	// }


	Game game = gameCreate();
	game = gameInit(game);

	game = gameRun(game);

	gameDestroy(game);
	
}