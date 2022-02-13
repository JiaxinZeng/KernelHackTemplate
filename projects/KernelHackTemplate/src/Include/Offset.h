#ifndef K_HACK_TEMPLATE_OFFSET
#define K_HACK_TEMPLATE_OFFSET

#include <ntdef.h>

ULONG32 offset_process_user_directory_table_base;
ULONG32 offset_process_object_table;
ULONG32 offset_process_unique_pid;
ULONG32 offset_process_thread_list_head;

BOOLEAN init_offsets();

#endif
