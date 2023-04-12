#ifndef __DEBUG_MEMORY__
	#define __DEBUG_MEMORY__

	#include <stdlib.h>
	#include "logger.h"
	void* debug_malloc(size_t size, char *file, int line)
	{
		loggerLog("allocating %d bytes at %s:%d", size, file, line);
		return malloc(size);
	}

	void debug_free(void* p, char *file, int line)
	{
		loggerLog("deallocating %s:%d", file, line);
		free(p);
	}

	#define malloc(x) debug_malloc((x), __FILE__, __LINE__)
	#define free(x) debug_free((x), __FILE__, __LINE__)
	#define calloc(x, y) debug_malloc((x) * (y), __FILE__, __LINE__)
#endif