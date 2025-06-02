#include "Array.h"
#include "Memory.h"
#include "Logger.h"

Array_Type Array_Create(U32 capacity, Size_t element_size) {
	void* data = Malloc(capacity * element_size);
	if (data == NULL) return NULL;
	Array_Type arr = Malloc(sizeof(Array_Type));
	if (arr == NULL) return NULL;

	arr->data = data;
	arr->element_size = element_size;
	arr->capacity = capacity;
	arr->size = 0;
	arr->print_fn = NULL;

	return arr;
}

void Array_Push(Array_Type arr, void* element) {
	if (arr->capacity <= arr->size) {
		arr->capacity *= 2;
		arr->data = Realloc(arr->data, (arr->capacity * arr->element_size));
		if (arr->data == NULL) {			
			PANIC("Realloc Failed");			
		}
	}

	void* dest = (U8*)arr->data + (arr->size * arr->element_size);
	Memcpy(dest, element, arr->element_size);
	arr->size++;
}

void Array_Print(Array_Type arr) {
	if (arr->print_fn == NULL) {
		LOG_WARNING("Print function was not provided");
		return;
	}

	for (int i = 0; i < arr->size; i++) {
		void* element = (U8*)arr->data + i * arr->element_size;
		arr->print_fn(element);
	}
}

void Array_SetPrintFn(Array_Type arr, ptrPrintFn print_fn) {
	arr->print_fn = print_fn;
}

void Array_Free(Array_Type arr) {
	if (arr->free_fn == NULL) {
		LOG_WARNING("Free function was not provided");
		return;
	}

	arr->free_fn(arr->data);
}

void Array_SetFreeFn(Array_Type arr, ptrFreeFn free_fn) {
	arr->free_fn = free_fn;
}