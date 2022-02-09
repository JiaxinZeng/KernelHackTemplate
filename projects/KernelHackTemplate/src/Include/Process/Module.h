#ifndef K_HACK_TEMPLATE_PROCESS_MODULE
#define K_HACK_TEMPLATE_PROCESS_MODULE

#include <ntdef.h>

void* get_module_base_address(const wchar_t* module_name, ULONG64 pid);

#endif
