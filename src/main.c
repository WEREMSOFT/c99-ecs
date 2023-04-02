#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/array.h"
#include "core/bitset.h"
#include "core/logger.h"

#include <SDL2/SDL.h>
#include "game.h"

int main()
{
	Game game = gameCreate();
	game = gameInit(game);

	game = gameRun(game);

	gameDestroy(game);
	
}