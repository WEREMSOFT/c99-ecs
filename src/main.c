#include <stdio.h>
#include <stdlib.h>
// #include "core/debugMemory.h"
#include <string.h>
#include <locale.h>

#include <stdint.h>

#include <SDL2/SDL.h>
#include "game.h"

int main()
{
	Game game = gameCreate();

	gameInit(&game);

	gameRun(&game);

	gameDestroy(game);
	
}