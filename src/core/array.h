#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	memcpy(&_this->data[_this->size * _this->dataTypeSize], element, _this->size);
	_this->size += 1;

}

void arrayDeleteElement(ArrayHeader *_this, int elementIndex)
{
	char lastElement[_this->dataTypeSize];
	memcpy(lastElement, &_this->data[_this->size * _this->dataTypeSize], _this->size);
	memcpy(&_this->data[_this->size * _this->dataTypeSize], &_this->data[elementIndex * _this->dataTypeSize], _this->size);
	memcpy(&_this->data[elementIndex * _this->dataTypeSize], lastElement, _this->size);
	_this->size--;
}
#endif