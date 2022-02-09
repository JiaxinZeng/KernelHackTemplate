#ifndef K_HACK_TEMPLATE_NT_APIS
#define K_HACK_TEMPLATE_NT_APIS

#include <ntifs.h>

NTKERNELAPI PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);

#endif
