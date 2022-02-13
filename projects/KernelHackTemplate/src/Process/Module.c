#include <ntifs.h>

#include "NtApi.h"
#include "Process/Module.h"

void* get_module_base_address(const wchar_t* module_name, const ULONG64 pid)
{
	if (!pid)
	{
		return NULL;
	}

	void* base = NULL;

	PEPROCESS process;
	if (PsLookupProcessByProcessId((HANDLE)pid, &process) != STATUS_SUCCESS)
	{
		return NULL;
	}

	if (wcscmp(module_name, L"") == 0)
	{
		base = PsGetProcessSectionBaseAddress(process);
		ObDereferenceObject(process);
	}

	return base;
}
