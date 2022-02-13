#ifndef K_HACK_TEMPLATE_NT_API
#define K_HACK_TEMPLATE_NT_API

#include <ntifs.h>

NTKERNELAPI PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);

#endif
