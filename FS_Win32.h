#pragma once
#include "Common.h"

U8*  Win32_ReadFile(const U8* path);

Bool Win32_WriteFile(const U8* path, const U8* buffer, Size_t size);
Bool Win32_FileExists(const U8* path);