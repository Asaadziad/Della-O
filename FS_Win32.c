#include "FS_Win32.h"
#include "Logger.h"
#include "Memory.h"
#include "windows.h"

U8* Win32_ReadFile(const U8* path) {
    HANDLE hFile;
    DWORD bytesRead, fileSize;
    BOOL success;

    hFile = CreateFileA(
        path,              
        GENERIC_READ,            
        FILE_SHARE_READ,        
        NULL,                   
        OPEN_EXISTING,          
        FILE_ATTRIBUTE_NORMAL,  
        NULL                    
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        LOG_ERROR("Failed to open file. Error: %lu\n", GetLastError());
        return NULL;
    }

    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {

        CloseHandle(hFile);
        return NULL;
    }

    U8* buffer = Malloc(fileSize + 1);
    if (buffer == NULL) {
        LOG_ERROR("Couldn't allocate buffer");
        return NULL;
    }

    success = ReadFile(
        hFile,         
        buffer,        
        fileSize,
        &bytesRead,    
        NULL          
    );

    if (!success) {
        LOG_ERROR("Failed to read file. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    buffer[bytesRead] = '\0';

    CloseHandle(hFile);

    return buffer;
}

Bool Win32_WriteFile(const U8* path, const U8* buffer, Size_t size) {

}

Bool Win32_FileExists(const U8* path) {}