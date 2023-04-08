#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __DEBUG_BUILD__

#include "core/array.h"
#include "core/bitset.h"
#include "core/logger.h"

#include <SDL2/SDL.h>
#include "game.h"

#include "core/logger.h"

int main()
{
	Game game = gameCreate();

	gameInit(&game);

	gameRun(&game);

	gameDestroy(game);
	
}