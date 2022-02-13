#include <ntifs.h>
#include <stdio.h>
#include <SEH/Memory.h>

// #include "Memory/Memory.h"
#include "Process/Process.h"
#include "Process/Module.h"
#include "Thread/Thread.h"
#include "Var.h"
#include "Offset.h"

void test(void* context)
{
	UNREFERENCED_PARAMETER(context);

	PETHREAD current_thread = PsGetCurrentThread();
	const ULONG64 current_tid = (ULONG64)PsGetCurrentThreadId();
	
	set_thread_persistent(current_thread);

	if (!set_thread_hidden(current_tid, current_thread,TRUE))
	{
		PsTerminateSystemThread(0);
	}

	// DbgPrint("[+] Thread hidden\n");

	PETHREAD fake_thread;
	PsLookupThreadByThreadId((HANDLE)current_tid, &fake_thread);

	// DbgPrint("current thread -> %p, new thread -> %p\n", current_thread, fake_thread);

	set_thread_hidden(current_tid, current_thread, FALSE);

	// DbgPrint("[+] Thread unhidden\n");

	PsTerminateSystemThread(0);
}

void DriverUnload(DRIVER_OBJECT* driver_object)
{
	UNREFERENCED_PARAMETER(driver_object);
}

BOOLEAN init_driver()
{
	return init_vars() && init_offsets();
}

NTSTATUS NTAPI DriverEntry(DRIVER_OBJECT* driver_object, UNICODE_STRING* registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	driver_object->DriverUnload = DriverUnload;

	if (!init_driver())
	{
		return STATUS_ACCESS_DENIED;
	}

	HANDLE thread_handle = create_thread(test, NULL);
	if (!thread_handle)
	{
		return STATUS_ACCESS_DENIED;
	}

	ZwClose(thread_handle);

	seh_memcpy((void*)0x1234567812345678, (const void*)0x8765432187654321, 100);
	// DbgPrint("seh_memcpy %d\n", seh_memcpy((void*)0x1234567812345678, (const void*)0x8765432187654321, 100));

	PEPROCESS process = get_process_by_name(L"Notepad.exe");
	if (!process)
	{
		return STATUS_ACCESS_DENIED;
	}

	swap_process(process, FALSE);

	char buf[4];
	memset(buf, 0, 4);
	memcpy(buf, get_module_base_address(L"", get_process_id(process)), 3);

	DbgPrint("%s\n", buf);

	swap_process(main_process, FALSE);

	ObfDereferenceObject(process);

	return STATUS_SUCCESS;
}
