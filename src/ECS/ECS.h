#ifndef __ECS_H__
#define __ECS_H__
#include <stdlib.h>
#include <string.h>

#define mMalloc malloc
#define mFree free

static int entityId = 0;

#define MAX_ENTITIES 10

// MISC DATATYPES
typedef struct 
{
	float x, y;
} Vector2f;

typedef struct 
{
	Vector2f position;
} PositionComponent;

// Array
typedef struct 
{
	int size, capacity, typeSize;
	char data[1];
} ArrayHeader;

ArrayHeader* arrayCreate(int capacity, int typeSize)
{
	int size = capacity * typeSize + sizeof(ArrayHeader);
	ArrayHeader* returnValue = (ArrayHeader *)mMalloc(size);
	returnValue->size = 0;
	returnValue->capacity = capacity;
	returnValue->typeSize = typeSize;
	return returnValue;
}

void arrayDestroy(ArrayHeader* _this)
{
	mFree(_this);
}

void arrayAddElement(ArrayHeader* _this, void* element)
{
	int nextElement = _this->size * _this->capacity;
	_this->size++;
	memcpy(&_this->data[nextElement], element, _this->typeSize);
}

void *arrayGetElementAt(ArrayHeader* _this, int elementPosition)
{
	return _this->data[elementPosition * _this->typeSize];
}

// ENTITY
typedef struct {
	int Id;
} Entity;

Entity entityCreate()
{
	entityId++;
	Entity returnValue = {.Id = entityId};
	return returnValue;
}

// COMPONENTS
typedef struct
{
	Vector2f velocity;
} RigidBodyComponent;

// REGISTRY
typedef struct 
{
	Entity entities[MAX_ENTITIES];
	PositionComponent positionComponents[MAX_ENTITIES];
	RigidBodyComponent rigidBodyComponents[MAX_ENTITIES];
} Registry;

Registry registryCreate()
{
	Registry returnValue = {0};
	return returnValue;
}

#endif