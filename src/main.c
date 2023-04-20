#include <stdio.h>
#include <stdlib.h>
#include "core/debugMemory.h"
#include <string.h>
#include <locale.h>

#include <stdint.h>

#include <SDL2/SDL.h>
#include "game.h"

int main()
{
	// loggerLog("size of header %d", sizeof(ArrayHeader));
	// ArrayHeader* array = arrayCreateAndInitToZero(1, sizeof(ArrayHeader));
	// ArrayHeader* value;
		
	// for(int i = 0; i < 1000000000; i+=10000)
	// {
	// 	value = arrayGetElementOrCreateAt(&array, i);
	// 	array = arrayAddElementAt(array, &(ArrayHeader){.capacity = i * 19, .size = i, .dataTypeSize = 0xBEEF}, i);
	// }
	
	// arrayDestroy(array);

	// return 0;
	Game game = gameCreate();

	gameInit(&game);

	gameRun(&game);

	gameDestroy(game);
	
}