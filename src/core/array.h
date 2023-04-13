#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <assert.h>
#include <stdlib.h>
#include "logger.h"

#ifndef myMalloc
	#define myMalloc malloc
	#define myFree free
#endif

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

	#ifdef __DEBUG_BUILD__
		memset(&_this->data[0], -1, _this->capacity * _this->dataTypeSize);
	#endif
}

ArrayHeader* arrayAddElement(ArrayHeader* _this, const void* element)
{
	#ifdef __DEBUG_BUILD__
	assert(element > 1024 && "the pointer doesn't look like a pointer");
	#endif
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

ArrayHeader* arrayAddElementI(ArrayHeader* _this, int element)
{
	assert(_this->dataTypeSize == sizeof(int) && "array does not contains ints");
	_this = arrayAddElement(_this, &element);
	return _this;
}

ArrayHeader* arrayAddElementF(ArrayHeader* _this, float element)
{
	assert(_this->dataTypeSize == sizeof(float) && "array does not contains floats");
	_this = arrayAddElement(_this, &element);
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
	if(_this->size == index)
	{
		_this->size++;
	}

	if(_this->size < index)
	{
		_this->size = index + 1;
	}

	return _this;
}

void* arrayGetElementAt(ArrayHeader* _this, int index)
{
	assert(_this->size > -1 && "arrayGetElementAt negative index");
	assert(index < _this->size && "arrayGetElementAt index out of bounds");
	return &_this->data[index * _this->dataTypeSize];
}

int arrayGetElementAtI(ArrayHeader* _this, int index)
{
	assert(_this->dataTypeSize == sizeof(int) && "array does not contains ints");
	int *element = arrayGetElementAt(_this, index);
	return *element;
}

float arrayGetElementAtf(ArrayHeader* _this, int index)
{
	assert(_this->dataTypeSize == sizeof(float) && "array does not contains floats");
	float *element = arrayGetElementAt(_this, index);
	return *element;
}

void* arrayGetElementOrCreateAt(ArrayHeader** _this, int index)
{
	typedef struct 
	{
		char array[(*_this)->dataTypeSize];
	} Pivot;
		
	Pivot pivot;

	memset(pivot.array, 0, (*_this)->dataTypeSize);

	if(index >= (*_this)->size)
	{
		(*_this) = arrayAddElementAt(*_this, &pivot, index);
	}

	return arrayGetElementAt((*_this), index);
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

	#ifdef __DEBUG_BUILD__
		memset(&_this->data[lastIndex * _this->dataTypeSize], -1, _this->dataTypeSize);
	#endif

	_this->size--;
}
#endif