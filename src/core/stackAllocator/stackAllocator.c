// #include "stackAllocator.h"
// #include <stdlib.h>

// StackAllocator *stackAllocatorCreate(size_t size)
// {
//     size_t sizeToAllocate = size + sizeof(StackAllocatorHeader);
//     StackAllocator *_this = (StackAllocator *)malloc(sizeof(char) * sizeToAllocate);

//     if (!_this)
//     {
//         printf("Error creating stack allocator %s::%d\n", __FILE__, __LINE__);
//         return NULL;
//     }

//     _this->header.capacity = size;
//     return _this;
// }

// void *stackAllocatorAlloc(StackAllocator *_this, size_t size)
// {
//     if (_this->header.length + size > _this->header.capacity)
//     {
//         // TODO: realloc will invalidate all the pointers provided before.
//         // For now, we just abort the program, in the future we should create
//         // A handle table
//         printf("Allocator ran out of space.\nActual capacity %ld\nRequired capacity %ld\n", _this->header.capacity, _this->header.capacity + size);
//         return NULL;
//     }
//     void *returnValue = &_this->data[_this->header.length];
//     _this->header.length += size;
//     _this->header.allocationCount++;
//     return returnValue;
// }

// void stackAllocatorReset(StackAllocator *_this)
// {
//     _this->header.length = 0;
// }

// void stackAllocatorFree(StackAllocator *_this)
// {
//     _this->header.allocationCount--;
//     if (_this->header.allocationCount <= 0)
//     {
//         stackAllocatorReset(_this);
//     }
// }

// void *stackAllocatorRealloc(StackAllocator *_this, void *pointer, size_t size)
// {
//     void *returnValue = stackAllocatorAlloc(_this, size);

//     if (pointer != NULL)
//         memcpy(returnValue, pointer, size);

//     return returnValue;
// }
