#include "Memory_Win32.h"
#include "Logger.h"

#include <windows.h>

HANDLE g_processHeap = NULL;

void* Win32_Malloc(Size_t size){
	if (g_processHeap == NULL) {
		g_processHeap = GetProcessHeap();
	} 
	return HeapAlloc(g_processHeap, 0, size);
}

void* Win32_Realloc(void* block, Size_t size) {

	void* new_block = HeapReAlloc(g_processHeap, 0, block, size);
	if (!new_block) {
		DWORD err = GetLastError();
		LOG_ERROR("HeapReAlloc failed with error code %lu\n", err);
		return NULL;
	}
}

void  Win32_Free(void* block){
	HeapFree(g_processHeap, 0, block);
}

void Win32_Memcpy(void* dest, const void* src, Size_t length){
	RtlCopyMemory(dest, src, length);
}

void Win32_Memmove(void* dest, const void* src, Size_t length){
	RtlMoveMemory(dest, src, length);
}