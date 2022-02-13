#include <ntifs.h>

#include "Var.h"

BOOLEAN init_vars()
{
	main_process = IoGetCurrentProcess();
	if (!main_process)
	{
		return FALSE;
	}

	memset(persistent_threads, 0, sizeof persistent_threads);
	persistent_thread_count = 0;

	RTL_OSVERSIONINFOW ver;
	memset(&ver, 0, sizeof ver);
	if (!NT_SUCCESS(RtlGetVersion(&ver)))
	{
		return FALSE;
	}
	current_win_build_number = ver.dwBuildNumber;

	ExInitializeFastMutex(&set_thread_hidden_lock);
	ExInitializeFastMutex(&enum_psp_cid_table_lock);
	ExInitializeFastMutex(&get_process_by_name_lock);
	ExInitializeFastMutex(&set_thread_persistent_lock);

	return TRUE;
}
