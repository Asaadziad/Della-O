#pragma once
#include "Common.h"

void* Win32_Malloc(Size_t size);
void  Win32_Free(void* block);
void* Win32_Realloc(void* block, Size_t size);


void Win32_Memcpy(void* dest, const void* src, Size_t length);
void Win32_Memmove(void* dest, const void* src, Size_t length);