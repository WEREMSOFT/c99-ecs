#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <assert.h>
#include <stdlib.h>

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
	ArrayHeader* returnValue = myMalloc(size);

	memset(returnValue->data, 0, capacity * dataTypeSize);
	
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

ArrayHeader* arrayAddElement(ArrayHeader* _this, const void* element)
{

	if(_this->size == _this->capacity)
	{
		ArrayHeader* biggerArray = arrayCreate(_this->capacity * 2, _this->dataTypeSize);
		memcpy(biggerArray->data, _this->data, _this->capacity * _this->dataTypeSize);
		biggerArray->size = _this->size;
		arrayDestroy(_this);
		_this = biggerArray;
	}

	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;


	Pivot* pivot = (Pivot *)&_this->data[_this->size * _this->dataTypeSize];

	*pivot = *((Pivot*)element);

	_this->size += 1;
	return _this;
}

ArrayHeader* arrayAddElementAt(ArrayHeader* _this, const void* element, int index)
{
	if(index >= _this->capacity)
	{
		ArrayHeader* biggerArray = arrayCreateAndInitToZero(index + 1, _this->dataTypeSize);
		memcpy(biggerArray->data, _this->data, _this->capacity * _this->dataTypeSize);
		arrayDestroy(_this);
		_this = biggerArray;
		_this->size = index;
	}

	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;

	Pivot* pivot = (Pivot *)&_this->data[index * _this->dataTypeSize];

	*pivot = *((Pivot*)element);
	
	// The size of the array must be at least equal to index;
	_this->size = index + 1;

	return _this;
}

void* arrayGetElementAt(ArrayHeader* _this, int index)
{
	assert(index < _this->size && "arrayGetElementAt index out of bounds");
	return &_this->data[index * _this->dataTypeSize];
}

void* arrayGetElementOrCreateAt(ArrayHeader* _this, int index)
{
	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;
		
	Pivot pivot;

	memset(pivot.array, 0, _this->dataTypeSize);

	if(index >= _this->size)
	{
		_this = arrayAddElementAt(_this, &pivot, index);
	}

	return arrayGetElementAt(_this, index);
}

void arrayDeleteElement(ArrayHeader *_this, int elementIndex)
{
	if(_this->size == 0 || elementIndex >= _this->size) return;
	
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