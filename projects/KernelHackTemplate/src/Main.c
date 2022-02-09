#include <ntifs.h>
#include <stdio.h>

// #include "Memory/Memory.h"
#include "Process/Process.h"
#include "Process/Module.h"
#include "Thread/Thread.h"
#include "NtApis.h"
#include "Var.h"

void test(void* context)
{
	UNREFERENCED_PARAMETER(context);

	PETHREAD current_thread = PsGetCurrentThread();
	const ULONG64 current_tid = (ULONG64)PsGetCurrentThreadId();
	
	persistent_threads[persistent_thread_count++] = current_thread;
	
	if (!set_thread_hidden(current_tid, current_thread,TRUE))
	{
		PsTerminateSystemThread(0);
	}

	DbgPrint("[+] Thread hidden\n");

	PETHREAD fake_thread;
	PsLookupThreadByThreadId((HANDLE)current_tid, &fake_thread);

	DbgPrint("current thread -> %p, new thread -> %p\n", current_thread, fake_thread);

	set_thread_hidden(current_tid, current_thread, FALSE);

	DbgPrint("[+] Thread unhidden\n");

	PsTerminateSystemThread(0);
}

void DriverUnload(DRIVER_OBJECT* driver_object)
{
	UNREFERENCED_PARAMETER(driver_object);
}

NTSTATUS NTAPI DriverEntry(DRIVER_OBJECT* driver_object, UNICODE_STRING* registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	driver_object->DriverUnload = DriverUnload;

	if (!init_vars())
	{
		return STATUS_ACCESS_DENIED;
	}

	HANDLE thread_handle = create_thread(test, NULL);
	if (!thread_handle)
	{
		return STATUS_ACCESS_DENIED;
	}

	ZwClose(thread_handle);

	return STATUS_SUCCESS;
}
