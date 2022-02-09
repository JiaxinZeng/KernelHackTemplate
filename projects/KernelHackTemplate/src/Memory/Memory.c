#include <ntifs.h>

#include "Process/Process.h"
#include "Memory/AddressTranslator.h"
#include "Memory/PhysicalMemory.h"

#include "Memory/Memory.h"

NTSTATUS read_process_memory(const ULONG64 pid, void* address, void* allocated_buffer, const SIZE_T size, SIZE_T* read)
{
	PEPROCESS process;
	if (!pid)
	{
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &process);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	const ULONG_PTR process_dir_base = get_process_cr3(process);
	ObDereferenceObject(process);

	SIZE_T cur_offset = 0;
	SIZE_T total_size = size;
	while (total_size)
	{
		ULONG_PTR cur_phys_address = translate_linear_address_virtual_to_physical(
			process_dir_base,
			(ULONG64)address + cur_offset);
		if (!cur_phys_address)
		{
			return STATUS_UNSUCCESSFUL;
		}

		const ULONG64 read_size = min(PAGE_SIZE - (cur_phys_address & 0xFFF), total_size);
		SIZE_T bytes_read = 0;
		status = read_physical_address((void*)cur_phys_address,
		                               (void*)((ULONG64)allocated_buffer + cur_offset),
		                               read_size,
		                               &bytes_read);
		total_size -= bytes_read;
		cur_offset += bytes_read;
		if (status != STATUS_SUCCESS)
		{
			break;
		}
		if (bytes_read == 0)
		{
			break;
		}
	}

	*read = cur_offset;
	return status;
}
