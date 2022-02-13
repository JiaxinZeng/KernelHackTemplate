#include <ntifs.h>

#include "Function.h"

BOOLEAN seh_call(const fn_seh_call func)
{
	BOOLEAN result = TRUE;
	__try
	{
		func();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		result = FALSE;
	}
	return result;
}
