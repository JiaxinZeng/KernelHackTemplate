#ifndef K_HACK_TEMPLATE_VAR
#define K_HACK_TEMPLATE_VAR

#include <ntdef.h>

PEPROCESS main_process;
PETHREAD persistent_threads[16];
ULONG32 persistent_thread_count;
ULONG current_win_build_number;

FAST_MUTEX set_thread_hidden_lock;
FAST_MUTEX enum_psp_cid_table_lock;
FAST_MUTEX get_process_by_name_lock;
FAST_MUTEX set_thread_persistent_lock;

BOOLEAN init_vars();

#endif
