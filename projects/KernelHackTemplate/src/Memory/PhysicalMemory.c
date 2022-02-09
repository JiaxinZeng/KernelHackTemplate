#include <ntifs.h>

NTSTATUS read_physical_address(void* target_address, void* buffer, const SIZE_T size, SIZE_T* bytes_read)
{
	MM_COPY_ADDRESS address_to_read;
	address_to_read.PhysicalAddress.QuadPart = (LONG64)target_address;
	return MmCopyMemory(buffer, address_to_read, size, MM_COPY_MEMORY_PHYSICAL, bytes_read);
}
