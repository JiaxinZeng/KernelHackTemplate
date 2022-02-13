#include <ntifs.h>

#include "Var.h"
#include "WinVerDef.h"

#include "Offset.h"

static BOOLEAN get_process_user_directory_table_base_offset()
{
	if (current_win_build_number >= WINDOWS_2004)
	{
		offset_process_user_directory_table_base = 0x388U;
	}
	else if (current_win_build_number >= WINDOWS_1903)
	{
		offset_process_user_directory_table_base = 0x280U;
	}
	else if (current_win_build_number >= WINDOWS_1803)
	{
		offset_process_user_directory_table_base = 0x278U;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

static BOOLEAN get_process_object_table_offset()
{
	if (current_win_build_number >= WINDOWS_2004)
	{
		offset_process_object_table = 0x570U;
	}
	else if (current_win_build_number >= WINDOWS_1803)
	{
		offset_process_object_table = 0x418U;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

static BOOLEAN get_process_unique_pid_offset()
{
	if (current_win_build_number >= WINDOWS_2004)
	{
		offset_process_unique_pid = 0x440U;
	}
	else if (current_win_build_number >= WINDOWS_1903)
	{
		offset_process_unique_pid = 0x2E8U;
	}
	else if (current_win_build_number >= WINDOWS_1803)
	{
		offset_process_unique_pid = 0x2E0U;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

static BOOLEAN get_process_thread_list_head_offset(void)
{
	if (current_win_build_number >= WINDOWS_2004)
	{
		offset_process_thread_list_head = 0x5e0U;
	}
	else if (current_win_build_number >= WINDOWS_1803)
	{
		offset_process_thread_list_head = 0x488U;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOLEAN init_offsets()
{
	return get_process_user_directory_table_base_offset() && get_process_object_table_offset() &&
		get_process_unique_pid_offset() && get_process_thread_list_head_offset();
}
