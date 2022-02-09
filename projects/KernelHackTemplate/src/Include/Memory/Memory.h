#ifndef K_HACK_TEMPLATE_MEMORY_MEMORY
#define K_HACK_TEMPLATE_MEMORY_MEMORY

#include <ntdef.h>

NTSTATUS read_process_memory(ULONG64 pid, void* address, void* allocated_buffer, SIZE_T size, SIZE_T* read);

#endif
