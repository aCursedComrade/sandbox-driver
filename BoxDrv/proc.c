#include "proc.h"
#include "util.h"

#pragma alloc_text(INIT, BoxDrvProcRegister)
#pragma alloc_text(PAGE, BoxDrvProcUnload)
#pragma alloc_text(PAGE, BoxDrvProcMonitor)
#pragma alloc_text(PAGE, BoxDrvProcGetName)

// Global variables

extern BoxDrvState stateInfo;
PVOID ZwQueryInformationProcess = NULL;

// Definitions

NTSTATUS BoxDrvProcRegister() {
	return PsSetCreateProcessNotifyRoutine(BoxDrvProcMonitor, FALSE);
}

VOID BoxDrvProcUnload() {
	PAGED_CODE();

	NTSTATUS status = PsSetCreateProcessNotifyRoutine(BoxDrvProcMonitor, TRUE);
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to unregister process monitor\r\n"));
	}
}

VOID BoxDrvProcMonitor(_In_ HANDLE ppid, _In_ HANDLE pid, _In_ BOOLEAN create) {
	PAGED_CODE();

	if (create) {
		// If create is TRUE, its a process creation
		if (BoxDrvIsInWatchlist(ppid)) {
			KdPrint(("BoxDrv: BoxDrvProcMonitor: New process with PID %llu created by %llu added to watch list\r\n", (ULONG_PTR)pid, (ULONG_PTR)ppid));
			BoxDrvAddToWatchlist(pid);
		}
	}
	else {
		// If create is FALSE, its a process termination
		if (BoxDrvIsInWatchlist(pid)) {
			KdPrint(("BoxDrv: BoxDrvProcMonitor: Terminating process with PID %llu removed from watch list\r\n", (ULONG_PTR)pid));
			BoxDrvRemoveFromWatchlist(pid);
		}
	}
}

NTSTATUS BoxDrvProcGetName(_In_ PEPROCESS pProcess, _Out_ PUNICODE_STRING* pProcessImageName) {
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
