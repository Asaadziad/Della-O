#pragma once
#include "Common.h"

void* Malloc(Size_t size);
void Free(void* ptr);
void* Realloc(void* data, Size_t size);


void Memcpy(void* dest, const void* src, Size_t size);
void Memmove(void* dest, const void* src, Size_t size);