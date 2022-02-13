#include <ntifs.h>

#include "Utility/PspCidTable.h"
#include "Var.h"
#include "Offset.h"
#include "Register.h"

#include "Process/Process.h"

static struct
{
	const wchar_t* process_name;
	PEPROCESS process;
} get_process_by_name_context = {L"", NULL};

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
		ObfDereferenceObject(process);
		return TRUE;
	}

	PUNICODE_STRING str = NULL;
	SeLocateProcessImageName(process, &str);

	if (!str || !str->Length || !str->Buffer)
	{
		ObfDereferenceObject(process);
		return TRUE;
	}

	if (wcsstr(str->Buffer, get_process_by_name_context.process_name))
	{
		get_process_by_name_context.process = process;
		return FALSE;
	}

	ObfDereferenceObject(process);
	return TRUE;
}

PEPROCESS get_process_by_name(const wchar_t* name)
{
	if (!name)
	{
		return NULL;
	}

	ExAcquireFastMutex(&get_process_by_name_lock);

	get_process_by_name_context.process_name = name;
	get_process_by_name_context.process = NULL;
	enum_psp_cid_table(get_process_by_name_callback);

	PEPROCESS process = get_process_by_name_context.process;

	ExReleaseFastMutex(&get_process_by_name_lock);

	return process;
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
		return *(ULONG_PTR*)((PUCHAR)process + offset_process_user_directory_table_base);
	}
	return process_dir_base;
}

ULONG64 get_process_id(PEPROCESS process)
{
	if (!MmIsAddressValid(process))
	{
		return 0;
	}

	return *(ULONG64*)((PUCHAR)process + offset_process_unique_pid);
}

BOOLEAN is_process_alive(PEPROCESS process)
{
	if (!MmIsAddressValid(process))
	{
		return FALSE;
	}

	const ULONG_PTR object_table_address = *(ULONG_PTR*)((PUCHAR)process + offset_process_object_table);
	return object_table_address ? TRUE : FALSE;
}

void swap_process(PEPROCESS process, const BOOLEAN update_apc_state)
{
	PETHREAD thread = PsGetCurrentThread();

	if (update_apc_state)
	{
		const ULONG_PTR apc_state = *(ULONG_PTR*)((ULONG_PTR)thread + 0x98);
		*(PEPROCESS*)(apc_state + 0x20) = process;
	}

	const ULONG_PTR process_dir_base = *(ULONG_PTR*)((ULONG_PTR)process + 0x28);
	WRITE_CR3(process_dir_base);
}
