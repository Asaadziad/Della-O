#include "FS.h"
#include "FS_Win32.h"

U8* FS_ReadFile(const char* path) {
	U8* data = NULL;
#ifdef _WIN32
	data = Win32_ReadFile(path);
#endif
	return data;
}

Bool FS_WriteFile(const char* path, const char* buffer, Size_t size){}
Bool FS_FileExists(const char* path) {}