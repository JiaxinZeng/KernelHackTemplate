#include <ntifs.h>

#include "Utility/PspCidTable.h"
#include "Var.h"
#include "WinVerDef.h"

#include "Thread/Thread.h"

static ULONG32 get_process_thread_list_head_offset(void)
{
	switch (current_win_build_number)
	{
	case WINDOWS_1803:
	case WINDOWS_1809:
	case WINDOWS_1903:
	case WINDOWS_1909:
		return 0x488U;
	case WINDOWS_2004:
	case WINDOWS_20H2:
	case WINDOWS_21H1:
	case WINDOWS_21H2:
	default:
		return 0x5e0U;
	}
}

PETHREAD get_thread_by_tid(const ULONG64 tid)
{
	PETHREAD thread = NULL;
	if (!NT_SUCCESS(PsLookupThreadByThreadId((HANDLE)tid, &thread)))
	{
		return NULL;
	}
	return thread;
}

static struct
{
	ULONG64 crypt_object;
} get_thread_first_crypt_parameters = {0};

static BOOLEAN get_thread_first_crypt_callback(const ULONG64 id,
                                               ULONG64* address,
                                               const ULONG64 crypt_object,
                                               void* decrypt_object)
{
	UNREFERENCED_PARAMETER(address);

	for (ULONG32 i = 0; i < persistent_thread_count; i++)
	{
		if (decrypt_object == persistent_threads[i])
		{
			return TRUE;
		}
	}

	void* thread = get_thread_by_tid(id);
	if (thread)
	{
		get_thread_first_crypt_parameters.crypt_object = crypt_object;
		ObfDereferenceObject(thread);
		return FALSE;
	}
	return TRUE;
}

static ULONG64 get_thread_first_crypt_object()
{
	enum_psp_cid_table(get_thread_first_crypt_callback);
	return get_thread_first_crypt_parameters.crypt_object;
}

static struct
{
	ULONG64 tid;
	ULONG64 original_crypt_object;
	ULONG64 replaced_crypt_object;
} internal_hidden_threads[128];

static BOOLEAN internal_hidden_threads_initialized = FALSE;

static short internal_hidden_thread_count = 0;

static struct
{
	BOOLEAN hide;
	ULONG64 tid;
	BOOLEAN result;
} internal_set_thread_hidden_parameters = {FALSE, 0, FALSE};

static BOOLEAN internal_set_thread_hidden_callback(const ULONG64 id,
                                                   ULONG64* address,
                                                   const ULONG64 crypt_object,
                                                   void* decrypt_object)
{
	UNREFERENCED_PARAMETER(decrypt_object);

	if (id != internal_set_thread_hidden_parameters.tid)
	{
		return TRUE;
	}
	if (internal_set_thread_hidden_parameters.hide)
	{
		internal_hidden_threads[internal_hidden_thread_count].tid = internal_set_thread_hidden_parameters.tid;
		internal_hidden_threads[internal_hidden_thread_count].original_crypt_object = crypt_object;
		*address = get_thread_first_crypt_object();
		internal_hidden_threads[internal_hidden_thread_count].replaced_crypt_object = *address;
		internal_set_thread_hidden_parameters.result = TRUE;
	}
	else
	{
		for (int i = 0; i < internal_hidden_thread_count; i++)
		{
			if (internal_hidden_threads[i].tid == id)
			{
				*address = internal_hidden_threads[i].original_crypt_object;
				internal_set_thread_hidden_parameters.result = TRUE;
				break;
			}
		}
	}
	return FALSE;
}

static BOOLEAN internal_set_thread_hidden(const ULONG64 tid, const BOOLEAN hide)
{
	if (!internal_hidden_threads_initialized)
	{
		memset(internal_hidden_threads, 0, sizeof internal_hidden_threads);
		internal_hidden_threads_initialized = TRUE;
	}
	internal_set_thread_hidden_parameters.hide = hide;
	internal_set_thread_hidden_parameters.tid = tid;
	internal_set_thread_hidden_parameters.result = FALSE;
	enum_psp_cid_table(internal_set_thread_hidden_callback);
	if (!internal_set_thread_hidden_parameters.result)
	{
		return FALSE;
	}
	if (hide)
	{
		internal_hidden_thread_count++;
	}
	return TRUE;
}

static struct
{
	BOOLEAN hide;
	ULONG64 tid;
	PETHREAD thread;
	BOOLEAN result;
} internal_set_thread_hidden2_parameters = {FALSE, 0, NULL, FALSE};

static BOOLEAN internal_set_thread_hidden2_callback(const ULONG64 id,
                                                    ULONG64* address,
                                                    const ULONG64 crypt_object,
                                                    void* decrypt_object)
{
	UNREFERENCED_PARAMETER(address);
	UNREFERENCED_PARAMETER(crypt_object);
	UNREFERENCED_PARAMETER(decrypt_object);

	if (internal_set_thread_hidden2_parameters.tid != id)
	{
		return TRUE;
	}

	const PETHREAD thread = internal_set_thread_hidden2_parameters.thread;

	if (internal_set_thread_hidden2_parameters.hide)
	{
		struct _LIST_ENTRY* list_entry = (struct _LIST_ENTRY*)((ULONG_PTR)thread + 0x2f8);
		RemoveEntryList(list_entry);

		list_entry = (struct _LIST_ENTRY*)((ULONG_PTR)thread + 0x538);
		RemoveEntryList(list_entry);
	}
	else
	{
		struct _LIST_ENTRY* list_entry = (struct _LIST_ENTRY*)((ULONG_PTR)thread + 0x2f8);
		struct _LIST_ENTRY* list_head = (struct _LIST_ENTRY*)((ULONG_PTR)main_process + 0x30);
		InsertHeadList(list_head, list_entry);

		list_entry = (struct _LIST_ENTRY*)((ULONG_PTR)thread + 0x538);
		list_head = (struct _LIST_ENTRY*)((ULONG_PTR)main_process + get_process_thread_list_head_offset());
		InsertHeadList(list_head, list_entry);
	}
	internal_set_thread_hidden2_parameters.result = TRUE;

	return FALSE;
}

static BOOLEAN internal_set_thread_hidden2(const ULONG64 tid, const PETHREAD thread, const BOOLEAN hide)
{
	internal_set_thread_hidden2_parameters.hide = hide;
	internal_set_thread_hidden2_parameters.tid = tid;
	internal_set_thread_hidden2_parameters.thread = thread;
	internal_set_thread_hidden2_parameters.result = FALSE;
	enum_psp_cid_table(internal_set_thread_hidden2_callback);
	return internal_set_thread_hidden2_parameters.result;
}

HANDLE create_thread(const PKSTART_ROUTINE start, void* context)
{
	HANDLE thread_handle;
	OBJECT_ATTRIBUTES object_attributes;
	CLIENT_ID tid;
	InitializeObjectAttributes(&object_attributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL)
	if (!NT_SUCCESS(
		PsCreateSystemThread(&thread_handle, THREAD_ALL_ACCESS, &object_attributes, NULL, &tid, start, context)))
	{
		return NULL;
	}
	return thread_handle;
}

BOOLEAN set_thread_hidden(const ULONG64 tid, const PETHREAD thread, const BOOLEAN hide)
{
	UNREFERENCED_PARAMETER(thread);

	KIRQL irql;

	KeAcquireSpinLock(&set_thread_hidden_lock, &irql);

	const BOOLEAN result = internal_set_thread_hidden(tid, hide) && internal_set_thread_hidden2(tid, thread, hide);
	
	KeReleaseSpinLock(&set_thread_hidden_lock, irql);

	return result;
}
