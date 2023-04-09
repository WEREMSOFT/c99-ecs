#ifndef __EVENT_BUS_H__
#define __EVENT_BUS_H__
#include "core/array.h"

#define INITIAL_EVENT_LISTENER_CAPACITY 10

typedef enum
{
	EVENT_TYPE_KEY_DOWN,
	EVENT_TYPE_COUNT
} EventType;

typedef struct
{
	EventType type;
	void* data;
} Event;

typedef struct 
{
	ArrayHeader *eventListeners[EVENT_TYPE_COUNT];
} EventBus;

typedef void (*EventListener)(Event);

void eventBusAddEventListener(EventBus _this, EventType eventType, EventListener eventListener)
{
	_this.eventListeners[eventType] = arrayAddElement(_this.eventListeners[eventType], &eventListener);
}

EventBus eventBusCleanEventListeners(EventBus _this)
{
	for(int i = 0; i < EVENT_TYPE_COUNT; i++)
	{
		arrayClear(_this.eventListeners[i]);
	}
}


EventBus eventBusCreate(void)
{
	EventBus returnValue;
	for(int i = 0; i < EVENT_TYPE_COUNT; i++)
	{
		returnValue.eventListeners[i] = arrayCreate(INITIAL_EVENT_LISTENER_CAPACITY, sizeof(EventListener));
	}
	return returnValue;
}

void eventBusEmmitEvent(EventBus _this, Event event)
{
	for(int i = 0; i < _this.eventListeners[event.type]->size; i++)
	{
		void *test = arrayGetElementAt(_this.eventListeners[event.type], i);
		EventListener listener = *(EventListener*)test;
		listener(event);
	}
}
#endif

