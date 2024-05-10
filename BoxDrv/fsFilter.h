#pragma once
#include "base.h"

NTSTATUS BoxDrvFsFilterInit(_In_ PDRIVER_OBJECT pDriverObject);
NTSTATUS BoxDrvFsFilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS flags);

NTSTATUS BoxDrvFsQueryTakeDown(_In_  PCFLT_RELATED_OBJECTS fltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags);
