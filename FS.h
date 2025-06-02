#pragma once

#include "Common.h"

U8* FS_ReadFile(const char* path);

Bool FS_WriteFile(const char* path, const char* buffer, Size_t size);
Bool FS_FileExists(const char* path);