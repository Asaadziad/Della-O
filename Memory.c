#include "Memory.h"

#include "Memory_Win32.h"

#include "stdlib.h"

void* Malloc(Size_t size) {
	void* data = NULL;
#ifdef _WIN32
	data = Win32_Malloc(size);
#endif
	return data;
}

void* Realloc(void* data, Size_t size) {
	if (data == NULL) return NULL;
	void* tmpData = NULL;

#ifdef _WIN32
	tmpData = Win32_Realloc(data, size);
#endif

	return tmpData;
}

void Free(void* ptr) {
#ifdef _WIN32
	Win32_Free(ptr);
#endif
}

void Memcpy(void* dest, const void* src, int size) {
	if (dest == NULL) return NULL;

#ifdef _WIN32
	Win32_Memcpy(dest, src, size);
#endif
}