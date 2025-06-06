#pragma once
#include "Common.h"

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

typedef enum {
    ARCH_UNKOWN,
    ARCH_X86_64,
    ARCH_ARM,
    ARCH_ARM64,
    ARCH_INTEL_ITANIUM64,
    ARCH_INTEL86,
    ARCH_COUNT,
} PROCESSOR_ARCH;

typedef struct cpu_t {
    const U8* vendor_id;
    PROCESSOR_ARCH arch;
    U8 word_size;                // in bytes (4 for 32-bit, 8 for 64-bit)
    U32 number_of_processors;               // default memory alignment
   
    Bool supports_fma;
    Bool has_sse;                // SIMD support
    Bool has_avx;
} CPUInfo;


const U8* GetProcessorArchString(PROCESSOR_ARCH arch);

void DetectArch(CPUInfo* info);

void DebugCPUInfo(CPUInfo info);
void DeallocateCPUInfo(CPUInfo* info);