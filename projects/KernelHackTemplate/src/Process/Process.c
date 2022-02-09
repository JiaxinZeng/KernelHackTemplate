#include <ntifs.h>

#include "Utility/PspCidTable.h"
#include "WinVerDef.h"
#include "Var.h"
#include "Register.h"

#include "Process/Process.h"

static ULONG32 get_process_user_directory_table_base_offset()
{
	switch (current_win_build_number)
	{
	case WINDOWS_1803:
	case WINDOWS_1809:
		return 0x278U;
	case WINDOWS_1903:
	case WINDOWS_1909:
		return 0x280U;
	case WINDOWS_2004:
	case WINDOWS_20H2:
	case WINDOWS_21H1:
	case WINDOWS_21H2:
	default:
		return 0x388U;
	}
}

static ULONG32 get_process_object_table_offset()
{
	switch (current_win_build_number)
	{
	case WINDOWS_1803:
	case WINDOWS_1809:
	case WINDOWS_1903:
	case WINDOWS_1909:
		return 0x418U;
	case WINDOWS_2004:
	case WINDOWS_20H2:
	case WINDOWS_21H1:
	case WINDOWS_21H2:
	default:
		return 0x570U;
	}
}

static ULONG32 get_process_unique_pid_offset()
{
	switch (current_win_build_number)
	{
	case WINDOWS_1803:
	case WINDOWS_1809:
		return 0x2E0U;
	case WINDOWS_1903:
	case WINDOWS_1909:
		return 0x2E8U;
	case WINDOWS_2004:
	case WINDOWS_20H2:
	case WINDOWS_21H1:
	case WINDOWS_21H2:
	default:
		return 0x440U;
	}
}

static struct
{
	const wchar_t* process_name;
	PEPROCESS process;
} get_process_by_name_parameters = {L"", NULL};

static BOOLEAN get_process_by_name_callback(const ULONG64 id,
                                            ULONG64* address,
                                            ULONG64 crypt_object,
                                            void* decrypt_object)
{
	UNREFERENCED_PARAMETER(address);
	UNREFERENCED_PARAMETER(crypt_object);
	UNREFERENCED_PARAMETER(decrypt_object);

	PEPROCESS process;

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)id, &process)))
	{
		return TRUE;
	}

	if (!is_process_alive(process))
	{
		return TRUE;
	}

	PUNICODE_STRING str = NULL;
	SeLocateProcessImageName(process, &str);

	if (!str || !str->Length)
	{
		return TRUE;
	}

	if (wcsstr(str->Buffer, get_process_by_name_parameters.process_name))
	{
		get_process_by_name_parameters.process = process;
		return FALSE;
	}
	return TRUE;
}

PEPROCESS get_process_by_name(const wchar_t* name)
{
	if (!name)
	{
		return NULL;
	}

	get_process_by_name_parameters.process_name = name;
	get_process_by_name_parameters.process = NULL;
	enum_psp_cid_table(get_process_by_name_callback);
	return get_process_by_name_parameters.process;
}

ULONG_PTR get_process_cr3(PEPROCESS process)
{
	if (!MmIsAddressValid(process))
	{
		return 0;
	}

	const ULONG_PTR process_dir_base = *(ULONG_PTR*)((PUCHAR)process + 0x28U);
	if (!process_dir_base)
	{
		const ULONG32 user_dir_offset = get_process_user_directory_table_base_offset();
		const ULONG_PTR process_user_dir_base = *(ULONG_PTR*)((PUCHAR)process + user_dir_offset);
		return process_user_dir_base;
	}
	return process_dir_base;
}

ULONG64 get_process_id(PEPROCESS process)
{
	if (!MmIsAddressValid(process))
	{
		return 0;
	}

	const ULONG32 pid_offset = get_process_unique_pid_offset();
	return *(ULONG64*)((PUCHAR)process + pid_offset);
}

BOOLEAN is_process_alive(PEPROCESS process)
{
	if (!MmIsAddressValid(process))
	{
		return 0;
	}

	const ULONG32 obj_table_offset = get_process_object_table_offset();
	const ULONG_PTR object_table_address = *(ULONG_PTR*)((PUCHAR)process + obj_table_offset);
	return object_table_address ? TRUE : FALSE;
}

PEPROCESS swap_process(PEPROCESS process)
{
	PETHREAD thread = PsGetCurrentThread();
	const ULONG_PTR apc_state = *(ULONG_PTR*)((ULONG_PTR)thread + 0x98);
	PEPROCESS old_process = *(PEPROCESS*)(apc_state + 0x20);
	*(PEPROCESS*)(apc_state + 0x20) = process;
	const ULONG_PTR process_dir_base = *(ULONG_PTR*)((ULONG_PTR)process + 0x28);
	write_cr3(process_dir_base);
	return old_process;
}
