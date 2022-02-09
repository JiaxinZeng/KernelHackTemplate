#include <ntifs.h>

#include "Utility/PspCidTable.h"

static ULONG_PTR psp_cid_table_address = 0;

static fn_enum_psp_cid_table_callback parse_table1_callback = NULL;

static BOOLEAN get_psp_cid_table()
{
	UNICODE_STRING func_name;
	RtlInitUnicodeString(&func_name, L"PsLookupProcessByProcessId");
	const ULONG_PTR func_address = (ULONG_PTR)MmGetSystemRoutineAddress(&func_name);
	if (func_address == 0)
	{
		return FALSE;
	}

	ULONG_PTR entry_address = 0;
	for (int i = 0; i < 100; i++)
	{
		if (*(PUCHAR)(func_address + i) == 0xe8)
		{
			entry_address = func_address + i;
			break;
		}
	}
	if (entry_address != 0)
	{
		const int call_code = *(int*)(entry_address + 1);
		const ULONG_PTR call_jmp = entry_address + call_code + 5;
		for (int i = 0; i < 50; i++)
		{
			if (*(PUCHAR)(call_jmp + i) == 0x48 &&
				*(PUCHAR)(call_jmp + i + 1) == 0x8b &&
				*(PUCHAR)(call_jmp + i + 2) == 0x05)
			{
				const int mov_code = *(int*)(call_jmp + i + 3);
				const ULONG_PTR mov_jmp = call_jmp + i + mov_code + 7;
				psp_cid_table_address = mov_jmp;
				return TRUE;
			}
		}
	}
	else
	{
		for (int i = 0; i < 100; i++)
		{
			if (*(PUCHAR)(func_address + i) == 0x49 &&
				*(PUCHAR)(func_address + i + 1) == 0x8b &&
				*(PUCHAR)(func_address + i + 2) == 0xdc &&
				*(PUCHAR)(func_address + i + 3) == 0x48 &&
				*(PUCHAR)(func_address + i + 4) == 0x8b &&
				*(PUCHAR)(func_address + i + 5) == 0xd1 &&
				*(PUCHAR)(func_address + i + 6) == 0x48 &&
				*(PUCHAR)(func_address + i + 7) == 0x8b)
			{
				const int mov_code = *(int*)(func_address + i + 6 + 3);
				const ULONG64 mov_jmp = func_address + i + 6 + mov_code + 7;
				psp_cid_table_address = mov_jmp;
				return TRUE;
			}
		}
	}

	return FALSE;
}

static BOOLEAN parse_table1(const ULONG_PTR base_address, const int index1, const int index2)
{
	for (int i = 0; i < 256; i++)
	{
		if (!MmIsAddressValid((void*)(base_address + (ULONG64)i * 16)))
		{
			continue;
		}
		const ULONG64 recode = *(ULONG_PTR*)(base_address + (ULONG64)i * 16);
		const ULONG_PTR decode = (LONG64)recode >> 0x10 & 0xfffffffffffffff0;

		PEPROCESS process;
		PETHREAD thread;

		const ULONG64 id = i * 4ULL + 1024ULL * index1 + 512ULL * index2 * 1024;

		if (parse_table1_callback)
		{
			if (!parse_table1_callback(id, (ULONG64*)(base_address + (ULONG64)i * 16), recode, (void*)decode))
			{
				return FALSE;
			}
			continue;
		}

		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)id, &process)))
		{
			DbgPrint("PID:%llu, i:%d, address:%p, object:%p\n",
				id,
				i,
				(void*)(base_address + (ULONG64)i * 16),
				(void*)decode);
		}
		else if (NT_SUCCESS(PsLookupThreadByThreadId((HANDLE)id, &thread)))
		{
			DbgPrint("TID:%llu, i:%d, address:%p, object:%p\n",
				id,
				i,
				(void*)(base_address + (ULONG64)i * 16),
				(void*)decode);
		}
	}
	return TRUE;
}

static BOOLEAN parse_table2(const ULONG_PTR base_address, const int index2)
{
	for (int i = 0; i < 512; i++)
	{
		if (!MmIsAddressValid((void*)(base_address + (ULONG64)i * 8)))
		{
			continue;
		}
		if (!MmIsAddressValid((void*)*(ULONG_PTR*)(base_address + (ULONG64)i * 8)))
		{
			continue;
		}
		const ULONG64 base_address1 = *(ULONG_PTR*)(base_address + (ULONG64)i * 8);
		if (!parse_table1(base_address1, i, index2))
		{
			return FALSE;
		}
	}
	return TRUE;
}

static void parse_table3(const ULONG64 base_address)
{
	for (int i = 0; i < 512; i++)
	{
		if (!MmIsAddressValid((void*)(base_address + (ULONG64)i * 8)))
		{
			continue;
		}
		if (!MmIsAddressValid((void*)*(ULONG_PTR*)(base_address + (ULONG64)i * 8)))
		{
			continue;
		}
		const ULONG64 ul_base_address2 = *(ULONG_PTR*)(base_address + (ULONG64)i * 8);
		if (!parse_table2(ul_base_address2, i))
		{
			return;
		}
	}
}

BOOLEAN enum_psp_cid_table(const fn_enum_psp_cid_table_callback callback)
{
	if (!psp_cid_table_address && !get_psp_cid_table())
	{
		return FALSE;
	}

	parse_table1_callback = callback;

	ULONG64 table_code = *(ULONG_PTR*)(*(ULONG_PTR*)psp_cid_table_address + 8);
	const ULONG64 low2 = table_code & 3;
	switch (low2)
	{
	case 0:
		parse_table1(table_code & ~3, 0, 0);
		break;
	case 1:
		parse_table2(table_code & ~3, 0);
		break;
	case 2:
		parse_table3(table_code & ~3);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
