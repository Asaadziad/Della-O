#pragma once

#include "Array.h"

typedef struct compiler_t {
	U8* rData;
	Array_Type tokens;
} CompilerInfo;

void CompilerMain(const char* file_path);