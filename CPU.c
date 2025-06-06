#include "CPU.h"
#include "CPU_Win32.h"

void DetectArch(CPUInfo* info) {
#ifdef _WIN32
	Win32_DetectArch(info);
#endif
}

void DebugCPUInfo(CPUInfo info) {
	Print(
        "Vendor: %s\n"
        "Architecture: %s\n"
        "Word Size: %d\n"
        "Number of processors: %d\n"
        "Has SSE: %s\n"
        "Has AVX: %s\n",
        info.vendor_id, GetProcessorArchString(info.arch), info.word_size,
        info.number_of_processors, info.has_sse ? "true" : "false",
        info.has_avx ? "true" : "false"
    );
}

const U8* ProcessorArchStringTable[] = {
    [ARCH_UNKOWN] = {"UNKOWN"},
    [ARCH_X86_64] = {"X86_64"},
    [ARCH_ARM] = {"ARM"},
    [ARCH_ARM64] = {"ARM64"},
    [ARCH_INTEL_ITANIUM64] = {"INTEL_ITANIUM64"},
    [ARCH_INTEL86] = {"INTEL86"},
};

const U8* GetProcessorArchString(PROCESSOR_ARCH arch) {
    return ProcessorArchStringTable[arch];
}


void DeallocateCPUInfo(CPUInfo* info) {
    Free(info->vendor_id); 
}