#pragma once
#include "Common.h"

typedef (*ptrPrintFn)(void* element);
typedef (*ptrFreeFn)(void* ptr);
typedef struct array_t {
	U32 size;
	U32 capacity;
	Size_t element_size;
	void* data;	
	ptrPrintFn print_fn;
	ptrFreeFn  free_fn;
} *Array_Type;

Array_Type Array_Create(U32 capacity, Size_t element_size);
void Array_Push(Array_Type arr, void* element);

void Array_Print(Array_Type arr);
void Array_SetPrintFn(Array_Type arr, ptrPrintFn print_fn);

/* Free's all elements inside the array */
void Array_Free(Array_Type arr);
void Array_SetFreeFn(Array_Type arr, ptrFreeFn free_fn);
