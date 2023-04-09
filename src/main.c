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

void eventListener(Event evt)
{
	loggerLog("Event recibed %d", evt.type);
}

int main()
{
	EventBus eb = eventBusCreate();

	EventListener el = eventListener;

	eventBusAddEventListener(eb, EVENT_TYPE_KEY_DOWN, el);

	Event e = {0};

	e.type = EVENT_TYPE_KEY_DOWN;

	eventBusEmmitEvent(eb, e);

	return 0;
	Game game = gameCreate();

	gameInit(&game);

	gameRun(&game);

	gameDestroy(game);
	
}