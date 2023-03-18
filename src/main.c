#include <stdio.h>
#include <stdlib.h>

#define MAX_ENTITIES 10

#define myMalloc malloc
#define myFree free
// Array
typedef struct
{
	int size;
	int capacity;
	int dataTypeSize;
	char data[1];
} ArrayHeader;

void* arrayCreate(int capacity, int dataTypeSize)
{
	void* returnValue = myMalloc(sizeof(ArrayHeader) + capacity * dataTypeSize);
	ArrayHeader* header = (ArrayHeader*)returnValue;
	header->capacity = capacity;
	header->size = 0;
	return returnValue;
}

void arrayDestroy(void *array)
{
	myFree(array);
}

void arrayInsertElement(ArrayHeader* _this, void* element)
{
	_this->data[_this->size * _this->dataTypeSize] = *element;
	_this->size += 1;

}

// Entities
typedef struct 
{
	int entities[MAX_ENTITIES];
	int components[MAX_ENTITIES];
	int entitiesToComponentsMapping[MAX_ENTITIES];
} Registry;

typedef struct 
{
	int id;
	int x;
	int y;
} Entity;

int main()
{
	ArrayHeader* entities = (ArrayHeader*)arrayCreate(10, sizeof(Entity));
	entities->data[entities->size * ]
	printf("Hello World!!\n");
}