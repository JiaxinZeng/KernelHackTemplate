#ifndef K_HACK_TEMPLATE_PROCESS_PROCESS
#define K_HACK_TEMPLATE_PROCESS_PROCESS

#include <ntdef.h>

PEPROCESS get_process_by_name(const wchar_t* name);
ULONG_PTR get_process_cr3(PEPROCESS process);
ULONG64 get_process_id(PEPROCESS process);

BOOLEAN is_process_alive(PEPROCESS process);

void swap_process(PEPROCESS process, BOOLEAN update_apc_state);

#endif
