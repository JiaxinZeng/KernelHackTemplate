#ifndef K_HACK_TEMPLATE_UTILITY_SLEEP
#define K_HACK_TEMPLATE_UTILITY_SLEEP

#include <ntifs.h>

#define SLEEP_MILLISECOND(m) { \
	LARGE_INTEGER sleep_time; \
	sleep_time.QuadPart = -10LL * 1000 * (m); \
	KeDelayExecutionThread(KernelMode, FALSE, &sleep_time); \
}

#endif
