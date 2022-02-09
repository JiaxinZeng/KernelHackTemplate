#include <ntifs.h>

#include "Memory/PhysicalMemory.h"

#include "Memory/AddressTranslator.h"

#define PAGE_OFFSET_SIZE 12
static const ULONG64 page_mask = ~0xfull << 8 & 0xfffffffffull;

ULONG_PTR translate_linear_address_virtual_to_physical(ULONG_PTR directory_table_base, ULONG_PTR virtual_address)
{
	directory_table_base &= ~0xf;

	const ULONG64 page_offset = virtual_address & ~(~0ul << PAGE_OFFSET_SIZE);
	const ULONG64 pte = virtual_address >> 12 & 0x1ffll;
	const ULONG64 pt = virtual_address >> 21 & 0x1ffll;
	const ULONG64 pd = virtual_address >> 30 & 0x1ffll;
	const ULONG64 pdp = virtual_address >> 39 & 0x1ffll;

	SIZE_T read_size = 0;
	ULONG64 pdpe = 0;
	read_physical_address((void*)(directory_table_base + 8 * pdp), &pdpe, sizeof pdpe, &read_size);
	if (~pdpe & 1)
		return 0;

	ULONG64 pde = 0;
	read_physical_address((void*)((pdpe & page_mask) + 8 * pd), &pde, sizeof pde, &read_size);
	if (~pde & 1)
		return 0;

	/* 1GB large page, use pde's 12-34 bits */
	if (pde & 0x80)
		return (pde & (~0ull << 42 >> 12)) + (virtual_address & ~(~0ull << 30));

	ULONG_PTR pte_address = 0;
	read_physical_address((void*)((pde & page_mask) + 8 * pt), &pte_address, sizeof pte_address, &read_size);
	if (~pte_address & 1)
		return 0;

	/* 2MB large page */
	if (pte_address & 0x80)
		return (pte_address & page_mask) + (virtual_address & ~(~0ull << 21));

	virtual_address = 0;
	read_physical_address((void*)((pte_address & page_mask) + 8 * pte),
	                      &virtual_address,
	                      sizeof virtual_address,
	                      &read_size);
	virtual_address &= page_mask;

	if (!virtual_address)
		return 0;

	return virtual_address + page_offset;
}
