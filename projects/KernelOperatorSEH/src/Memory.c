#include <ntifs.h>

#include "Memory.h"

BOOLEAN seh_memcpy(void* dst, const void* src, const size_t bytes)
{
	BOOLEAN result = TRUE;
	__try
	{
		memcpy(dst, src, bytes);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		result = FALSE;
	}
	return result;
}

BOOLEAN seh_memset(void* dst, const int val, const size_t bytes)
{
	BOOLEAN result = TRUE;
	__try
	{
		memset(dst, val, bytes);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		result = FALSE;
	}
	return result;
}
