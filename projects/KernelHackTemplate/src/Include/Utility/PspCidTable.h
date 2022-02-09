#ifndef K_HACK_TEMPLATE_UTILITY_PSP_CID_TABLE
#define K_HACK_TEMPLATE_UTILITY_PSP_CID_TABLE

#include <ntdef.h>

typedef BOOLEAN (*fn_enum_psp_cid_table_callback)(ULONG64 id,
                                                  ULONG64* address,
                                                  ULONG64 crypt_object,
                                                  void* decrypt_object);

BOOLEAN enum_psp_cid_table(fn_enum_psp_cid_table_callback callback);

#endif
