#include "fsFilter.h"

#pragma alloc_text(INIT, BoxDrvFsFilterInit)
#pragma alloc_text(PAGE, BoxDrvFsFilterUnload)
#pragma alloc_text(PAGE, BoxDrvFsQueryTakeDown)

// Global vairables

CONST FLT_REGISTRATION fltRegistration = {
	sizeof(FLT_REGISTRATION),			// struct size
	FLT_REGISTRATION_VERSION,			// version
	0,									// flags
	NULL,								// context
	NULL,								// callbacks
	BoxDrvFsFilterUnload,				// unload routine
	NULL,								// InstanceSetup
	BoxDrvFsQueryTakeDown,				// IntanceQuryTakeDown
	NULL,								// InstanceTearDownStart
	NULL,								// InstanceTearDownComplete
	NULL,								// GenerateFileName
	NULL,								// GenerateDestinationFileName
	NULL,								// NormalizeNameComponent
};

// Function definitions

NTSTATUS BoxDrvFsFilterInit(_In_ PDRIVER_OBJECT pDriverObject) {
	UNREFERENCED_PARAMETER(pDriverObject);

	return STATUS_SUCCESS;
}
NTSTATUS BoxDrvFsFilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS flags) {
	UNREFERENCED_PARAMETER(flags);

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvFsQueryTakeDown(_In_  PCFLT_RELATED_OBJECTS fltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags) {
	UNREFERENCED_PARAMETER(fltObjects);
	UNREFERENCED_PARAMETER(flags);

	return STATUS_SUCCESS;
}
