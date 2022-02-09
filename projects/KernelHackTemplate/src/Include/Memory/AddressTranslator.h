#ifndef K_HACK_TEMPLATE_MEMORY_ADDRESS_TRANSLATOR
#define K_HACK_TEMPLATE_MEMORY_ADDRESS_TRANSLATOR

#include <ntdef.h>

ULONG_PTR translate_linear_address_virtual_to_physical(ULONG_PTR directory_table_base, ULONG_PTR virtual_address);

#endif
