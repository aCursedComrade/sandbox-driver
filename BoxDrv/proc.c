#include "proc.h"

#pragma alloc_text(PAGE, BoxDrvGetProcessName)

// Global variables

PVOID ZwQueryInformationProcess = NULL;

// Definitions

NTSTATUS BoxDrvGetProcessName(_In_ PEPROCESS pProcess, _Out_ PUNICODE_STRING* pProcessImageName) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(pProcess);
	UNREFERENCED_PARAMETER(pProcessImageName);
	PAGED_CODE();

	// TODO this process has involves resolving some undocumented functions
	// we will stick to PIDs for the time being (names are optional afterall)
	// type definitions and layout may have changed compared to below answers
	// - https://www.ntkernel.com/forums/topic/retrieving-full-process-image-path-name-in-kernel-mode/
	// - https://stackoverflow.com/a/39353474

	return status;
}
