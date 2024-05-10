#include "ioctl.h"
#include "util.h"

#pragma alloc_text(PAGE, BoxDrvIoEcho)
#pragma alloc_text(PAGE, BoxDrvIoReadList)
#pragma alloc_text(PAGE, BoxDrvIoWriteList)

// Global variables

extern BoxDrvState stateInfo;

// Function definitions

NTSTATUS BoxDrvIoEcho(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PCHAR pInputBuffer = NULL, pOutputBuffer = NULL;
	ULONG bufInSize = 0, bufOutsize = 0;
	UINT32 writeSize = 0;

	PAGED_CODE();

	KdPrint(("BoxDrv: BoxDrvIoEcho\r\n"));
	pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
	pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pInputBuffer && pOutputBuffer) {
		bufInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
		bufOutsize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

		if (BoxDrvIsStringTerminated(pInputBuffer, bufInSize)) {
			if (bufOutsize >= bufInSize) {
				RtlCopyMemory(pOutputBuffer, pInputBuffer, bufInSize);
				status = STATUS_SUCCESS;
				writeSize = bufInSize;
			}
			else {
				KdPrint(("BoxDrv: BoxDrvIoEcho: Output buffer is too small!\r\n"));
				status = STATUS_BUFFER_TOO_SMALL;
			}
		}
	}

	*dataWritten = writeSize;
	return status;
}

NTSTATUS BoxDrvIoReadList(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PVOID pOutputBuffer = NULL;
	ULONG bufOutsize = 0;
	UINT32 writeSize = 0;

	PAGED_CODE();

	KdPrint(("BoxDrv: BoxDrvIoReadList\r\n"));
	pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pOutputBuffer) {
		bufOutsize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
		ULONG stateSize = sizeof(stateInfo.watchlist);

		if (bufOutsize >= stateSize) {
			RtlCopyMemory(pOutputBuffer, stateInfo.watchlist, stateSize);
			status = STATUS_SUCCESS;
			writeSize = stateSize;
		}
		else {
			KdPrint(("BoxDrv: BoxDrvReadList: Output buffer is too small!\r\n"));
			status = STATUS_BUFFER_TOO_SMALL;
		}
	}

	*dataWritten = writeSize;
	return status;
}

NTSTATUS BoxDrvIoWriteList(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PVOID pInputBuffer = NULL;
	ULONG bufInsize = 0;
	UINT32 writeSize = 0;

	PAGED_CODE();

	KdPrint(("BoxDrv: BoxDrvIoWriteList\r\n"));
	pInputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pInputBuffer) {
		bufInsize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
		ULONG typeSize = sizeof(TRACK_TYPE);

		if (bufInsize <= typeSize) {
			if (BoxDrvAddToWatchlist(*(HANDLE*)pInputBuffer)) {
				KdPrint(("BoxDrv: BoxDrvIoWriteList: Added PID %llu to the watch list\r\n", *(ULONG_PTR*)pInputBuffer));
				status = STATUS_SUCCESS;
				writeSize = typeSize;
			}
			else {
				KdPrint(("BoxDrv: BoxDrvIoWriteList: No space left in state list!\r\n"));
				status = STATUS_BUFFER_OVERFLOW;
			}
		}
		else {
			KdPrint(("BoxDrv: BoxDrvIoWriteList: Input buffer is larger than the expected size!\r\n"));
			status = STATUS_BUFFER_OVERFLOW;
		}
	}

	*dataWritten = writeSize;
	return status;
}
