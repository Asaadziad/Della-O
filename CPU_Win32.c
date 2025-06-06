#include "CPU_Win32.h"
#include "Memory.h"
#include <windows.h>

void Win32_DetectArch(CPUInfo* info) {
	SYSTEM_INFO system_info;
	
	GetSystemInfo(&system_info);
	U8 processor_architecture = system_info.wProcessorArchitecture;
	switch (processor_architecture) {
	case PROCESSOR_ARCHITECTURE_AMD64:
		info->arch = ARCH_X86_64; // x64(AMD or Intel)
		break; 
	case PROCESSOR_ARCHITECTURE_ARM:
		info->arch = ARCH_ARM;
		break;

	case PROCESSOR_ARCHITECTURE_ARM64:
		info->arch = ARCH_ARM64;
		break;			
			
	case PROCESSOR_ARCHITECTURE_IA64:
		info->arch = ARCH_INTEL_ITANIUM64;
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		info->arch = ARCH_INTEL86;
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		info->arch = ARCH_UNKOWN;
		break;		
	}

	info->word_size = sizeof(void*);
	info->number_of_processors = system_info.dwNumberOfProcessors;
	info->has_sse = Win32_HasSSE();
	info->has_avx = Win32_HasAVX();

	U8* vendor = Malloc(16);
	if (vendor == NULL) {
		 // ?? 
	}

	Win32_GetVendor(vendor);
	info->vendor_id = vendor;
	
}

static void CPUID(int func, unsigned regs[4]) {
	
	unsigned out[4];
	__cpuidex(out, func, 0);
	

	regs[_REG_EAX] = out[0];
	regs[_REG_EBX] = out[1];
	regs[_REG_ECX] = out[2];
	regs[_REG_EDX] = out[3];
}

void Win32_GetVendor( U8* vendor ) {
	unsigned out[4];
	

	CPUID(0, out);

	((unsigned*)vendor)[0] = out[1]; // EBX
	((unsigned*)vendor)[1] = out[3]; // EDX
	((unsigned*)vendor)[2] = out[2]; // ECX
	vendor[12] = 0;                  // Null-terminate

}

Bool Win32_HasSSE(){ 
	unsigned regs[4];

	CPUID(1, regs);

	// bit 25 of EDX means SSE support
	if (regs[_REG_EDX] & (1 << 25)) {
		return TRUE;
	}

	return FALSE;
}

Bool Win32_HasAVX() {
	unsigned regs[4];

	CPUID(1, regs);

	// bit 28 of ECX means AVX support
	if (regs[_REG_ECX] & (1 << 28)) {
		return TRUE;
	}

	return FALSE;
}