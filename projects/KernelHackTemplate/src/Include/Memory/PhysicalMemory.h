#ifndef K_HACK_TEMPLATE_MEMORY_PHYSICAL_MEMORY
#define K_HACK_TEMPLATE_MEMORY_PHYSICAL_MEMORY

#include <ntdef.h>

NTSTATUS read_physical_address(void* target_address, void* buffer, SIZE_T size, SIZE_T* bytes_read);

#endif
