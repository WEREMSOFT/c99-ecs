#ifndef __STATIC_ALLOC_H__
#define __STATIC_ALLOC_H__

#include "stackAllocator.h"

void *allocStatic(size_t size);
void *reallocStatic(void *_this, size_t size);
void freeStatic(void *_this);
void staticAllocatorDestroy();
#endif

#ifdef __STATIC_ALLOC_IMPLEMENTATION__
#undef __STATIC_ALLOC_IMPLEMENTATION__

static StackAllocator *stackAllocator;

void staticAllocatorInit(long int capacity)
{
    stackAllocator = stackAllocatorCreate(capacity);
}

void *allocStatic(size_t size)
{
    void *returnValue = stackAllocatorAlloc(stackAllocator, size);
    return returnValue;
}
void *reallocStatic(void *_this, size_t size)
{
    void *returnValue = stackAllocatorRealloc(stackAllocator, _this, size);
    return returnValue;
}

void freeStatic(void *_this)
{
    if (_this == NULL)
        return;
    stackAllocatorFree(stackAllocator);
}

void staticAllocatorDestroy()
{
	printf("destroying static allocator, allocated memory size %d bytes\n", stackAllocator->header.length);
    free(stackAllocator);
}

int getMemoryUsage()
{
	return stackAllocator->header.length;
}

#endif