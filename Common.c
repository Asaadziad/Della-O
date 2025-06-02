#include "Common.h"

#include "stdio.h"

void Print(const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}