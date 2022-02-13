#ifndef KERNEL_OPERATOR_SEH_FUNCTION
#define KERNEL_OPERATOR_SEH_FUNCTION

typedef void(*fn_seh_call)();

BOOLEAN seh_call(fn_seh_call func);

#endif
