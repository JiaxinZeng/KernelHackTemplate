#ifndef K_HACK_TEMPLATE_THREAD_THREAD
#define K_HACK_TEMPLATE_THREAD_THREAD

#include <ntdef.h>

HANDLE create_thread(PKSTART_ROUTINE start, void* context);

BOOLEAN set_thread_hidden(ULONG64 tid, PETHREAD thread, BOOLEAN hide);
void set_thread_persistent(PETHREAD thread);

PETHREAD get_thread_by_tid(ULONG64 tid);

#endif
