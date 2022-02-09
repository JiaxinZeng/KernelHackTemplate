#ifndef K_HACK_TEMPLATE_VAR
#define K_HACK_TEMPLATE_VAR

#include <ntdef.h>

PEPROCESS main_process;
PETHREAD persistent_threads[16];
ULONG32 persistent_thread_count;
ULONG current_win_build_number;

KSPIN_LOCK set_thread_hidden_lock;

BOOLEAN init_vars(void);

#endif
