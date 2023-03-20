#ifndef __ARRAY_H__
#define __ARRAY_H__

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

void arrayDestroy(void *array)
{
	myFree(array);
}

void arrayAddElement(ArrayHeader* _this, void* element)
{
	if(_this->size == _this->capacity) return;
	typedef struct 
	{
		char array[_this->dataTypeSize];
	} Pivot;

	Pivot* pivot = (Pivot *)&_this->data[_this->size * _this->dataTypeSize];

	*pivot = *((Pivot*)element);

	_this->size += 1;
}

void* arrayGetElement(ArrayHeader* _this, int index)
{
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