#ifndef __DEBUG_MEMORY__
#define __DEBUG_MEMORY__

	#include <stdlib.h>
	#include <assert.h>
	#include "logger.h"

	typedef struct 
	{
		void* pointer;
		size_t size;
	} MemInfo;
	static int meminfoCount = 0;
	static MemInfo memoryInformations[100];

	static int totalAllocatedMemory = 0;

	void* debug_realloc(void *p, size_t size, char *file, int line)
	{
		totalAllocatedMemory += size;
		loggerLog("reallocating %d bytes at %s:%d", size, file, line);
		loggerLog("Total allocated memory %zu", totalAllocatedMemory);
		MemInfo memInfo = {0};
		memInfo.pointer = realloc(p, size);
		memInfo.size = size;

		memoryInformations[meminfoCount++] = memInfo;

		return memInfo.pointer;
	}

	void* debug_malloc(size_t size, char *file, int line)
	{
		totalAllocatedMemory += size;
		loggerLog("allocating %d bytes at %s:%d", size, file, line);
		loggerLog("Total allocated memory %zu", totalAllocatedMemory);
		MemInfo memInfo = {0};
		memInfo.pointer = malloc(size);
		memInfo.size = size;

		memoryInformations[meminfoCount++] = memInfo;

		return memInfo.pointer;
	}

	void* debug_calloc(size_t nmemb, size_t size, char *file, int line)
	{
		totalAllocatedMemory += size;
		loggerLog("allocating %d bytes at %s:%d", size, file, line);
		loggerLog("Total allocated memory %zu", totalAllocatedMemory);
		MemInfo memInfo = {0};
		memInfo.pointer = calloc(nmemb, size);
		memInfo.size = nmemb * size;

		memoryInformations[meminfoCount++] = memInfo;

		return memInfo.pointer;
	}

	void debug_free(void* p, char *file, int line)
	{
		int location = -1;
		for(int i = 0; i<meminfoCount; i++)
		{
			if(memoryInformations[i].pointer == p)
			{
				location = i;
				break;
			}
		}

		assert(location > -1 && "pointer not found, possible double free?");
		loggerLog("deallocating %d bytes - %s:%d", memoryInformations[location].size, file, line);

		memoryInformations[location] = memoryInformations[meminfoCount-1];
		meminfoCount--;
		free(p);
	}

	int getTotalAllocatedMemory()
	{
		int result = 0;
		for(int i = 0; i < meminfoCount; i++)
		{
			result += memoryInformations[i].size;
		}
		return result;
	}

	#define malloc(x) debug_malloc((x), __FILE__, __LINE__)
	#define realloc(x, y) debug_realloc((x), (y), __FILE__, __LINE__)
	#define free(x) debug_free((x), __FILE__, __LINE__)
	#define calloc(x, y) debug_calloc((x),  (y), __FILE__, __LINE__)
#endif