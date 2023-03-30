#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <assert.h>

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

ArrayHeader* arrayCreate(int capacity, int dataTypeSize)
{
	void* returnValue = myMalloc(sizeof(ArrayHeader) + capacity * dataTypeSize);
	ArrayHeader* header = (ArrayHeader*)returnValue;
	header->capacity = capacity;
	header->size = 0;
	header->dataTypeSize = dataTypeSize;
	return returnValue;
}

ArrayHeader* arrayCreateAndInitToZero(int capacity, int dataTypeSize)
{
	size_t size = sizeof(ArrayHeader) + capacity * dataTypeSize;
	void* returnValue = myMalloc(size);
	char *memToZero = returnValue;
	for(int i = 0; i < size; i++)
	{
		memToZero[i] = 0;
	}

	ArrayHeader* header = (ArrayHeader*)returnValue;
	header->capacity = capacity;
	header->size = 0;
	header->dataTypeSize = dataTypeSize;
	return returnValue;
}

void arrayDestroy(void *array)
{
	myFree(array);
}

void arrayClear(ArrayHeader* _this)
{
	_this->size = 0;
}

void* arrayAddElement(ArrayHeader* _this, const void* element)
{
	assert(_this->size < _this->capacity && "Array run out of space. Increase capacity.");

	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;

	Pivot* pivot = (Pivot *)&_this->data[_this->size * _this->dataTypeSize];

	*pivot = *((Pivot*)element);
	_this->size += 1;

	return pivot;
}

void* arrayAddElementAt(ArrayHeader* _this, const void* element, int index)
{
	assert(index < _this->capacity && "array out of bounds");

	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;

	Pivot* pivot = (Pivot *)&_this->data[index * _this->dataTypeSize];

	*pivot = *((Pivot*)element);
	
	// The size of the array must be at least equal to index;
	if(_this->size - 1 < index)
		_this->size = index + 1;

	return pivot;
}

void* arrayGetElementAt(ArrayHeader* _this, int index)
{
	assert(index < _this->size && "arrayGetElementAt index out of bounds");
	return &_this->data[index * _this->dataTypeSize];
}

void* arrayGetElementOrCreateAt(ArrayHeader* _this, int index)
{
	assert(index < _this->capacity && "arrayGetElementAtOrCreate index out of bounds");
	if(_this->size < index + 1)
		_this->size = index + 1;

	return &_this->data[index * _this->dataTypeSize];
}

void arrayDeleteElement(ArrayHeader *_this, int elementIndex)
{
	if(_this->size == 0) return;
	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;

	int lastIndex = _this->size - 1;
	Pivot* lastElement = (Pivot *)&_this->data[lastIndex * _this->dataTypeSize];
	*((Pivot *)&_this->data[elementIndex * _this->dataTypeSize]) = *lastElement;

	_this->size--;
}
#endif