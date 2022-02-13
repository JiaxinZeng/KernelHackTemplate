#ifndef KERNEL_OPERATOR_SEH_MEMORY
#define KERNEL_OPERATOR_SEH_MEMORY

BOOLEAN seh_memcpy(void* dst, const void* src, size_t bytes);
BOOLEAN seh_memset(void* dst, int val, size_t bytes);

#endif
