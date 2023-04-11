#ifndef __STACK_ALLOCATOR_H__
#define __STACK_ALLOCATOR_H__

/**
 * StackAllocator is a custom allocator that returns pointers to a memory arena and grows itself 
 * when is asked to reserve more memory than capacity.
 * 
 * Using this type of custom allocator has the following advantages over using malloc and free
 * in a clasical way:
 * 
 * 1. Allocate memory and free memory takes time because the operative system need to do bookkeeping
 * of the memory that was allocated. Using a StackAllocator with a reasonable gessed initial size
 * will avoid the cycle of alloc/free removing the performance penalty.
 * 
 * 2. Memory fragmentation is reduced, specially when working with a lot of small objects.
 * 
 * 3. You can handle process that requires allocate and free memory in a more maintainable way
 * you create the StackAllocator in at the start of the process(let's say, in the begining of the
 * asset loading routine) allocate how many objects you need, and they you free only the allocator.
 * Reducing the stress to the operative system and the memory fragmentation for your program.
 * 
 */

#include <string.h>
#include <stdio.h>

typedef struct
{
    size_t capacity;
    size_t length;
    int allocationCount;
} StackAllocatorHeader;

typedef struct
{
    StackAllocatorHeader header;
    char data[1];
} StackAllocator;

// StackAllocator *stackAllocatorCreate(size_t size);
// void *stackAllocatorAlloc(StackAllocator *_this, size_t size);
// void stackAllocatorReset(StackAllocator *_this);
// void stackAllocatorFree(StackAllocator *_this);
// void *stackAllocatorRealloc(StackAllocator *_this, void *pointer, size_t size);


StackAllocator *stackAllocatorCreate(size_t size)
{
    size_t sizeToAllocate = size + sizeof(StackAllocatorHeader);
    StackAllocator *_this = (StackAllocator *)malloc(sizeof(char) * sizeToAllocate);

    if (!_this)
    {
        printf("Error creating stack allocator %s::%d\n", __FILE__, __LINE__);
        return NULL;
    }

    _this->header.capacity = size;
    return _this;
}

void *stackAllocatorAlloc(StackAllocator *_this, size_t size)
{
    if (_this->header.length + size > _this->header.capacity)
    {
        // TODO: realloc will invalidate all the pointers provided before.
        // For now, we just abort the program, in the future we should create
        // A handle table
        printf("Allocator ran out of space.\nActual capacity %ld\nRequired capacity %ld\n", _this->header.capacity, _this->header.capacity + size);
        return NULL;
    }
    void *returnValue = &_this->data[_this->header.length];
    _this->header.length += size;
    _this->header.allocationCount++;
    return returnValue;
}

void stackAllocatorReset(StackAllocator *_this)
{
    _this->header.length = 0;
}

void stackAllocatorFree(StackAllocator *_this)
{
    _this->header.allocationCount--;
    if (_this->header.allocationCount <= 0)
    {
        stackAllocatorReset(_this);
    }
}

void *stackAllocatorRealloc(StackAllocator *_this, void *pointer, size_t size)
{
    void *returnValue = stackAllocatorAlloc(_this, size);

    if (pointer != NULL)
        memcpy(returnValue, pointer, size);

    return returnValue;
}


#endif