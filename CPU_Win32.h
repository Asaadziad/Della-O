#pragma once

#include "CPU.h"

void Win32_GetVendor(U8* processor_name);
void Win32_DetectArch(CPUInfo* info);
Bool Win32_HasSSE();
Bool Win32_HasAVX();