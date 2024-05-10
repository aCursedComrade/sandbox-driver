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

	pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
	pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pInputBuffer && pOutputBuffer) {
		bufInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
		bufOutsize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

		if (BoxDrvIsStringTerminated(pInputBuffer, bufInSize)) {
			KdPrint(("BoxDrv: BoxDrvIoEcho: Echo message: %s\r\n", pInputBuffer));
			KdPrint(("BoxDrv: BoxDrvIoEcho: In buffer size: %lu\r\n", bufInSize));
			KdPrint(("BoxDrv: BoxDrvIoEcho: Out buffer size: %lu\r\n", bufOutsize));

			if (bufOutsize >= bufInSize) {
				RtlCopyMemory(pOutputBuffer, pInputBuffer, bufInSize);
				status = STATUS_SUCCESS;
				writeSize = bufInSize;
			}
			else {
				KdPrint(("BoxDrv: BoxDrvIoEcho: Output buffer is too small!"));
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

	pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pOutputBuffer) {
		bufOutsize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
		ULONG stateSize = sizeof(stateInfo.watchlist);

		KdPrint(("BoxDrv: BoxDrvReadList: Out buffer size: %lu\r\n", bufOutsize));
		KdPrint(("BoxDrv: BoxDrvReadList: State size: %lu\r\n", stateSize));

		if (bufOutsize >= stateSize) {
			RtlCopyMemory(pOutputBuffer, stateInfo.watchlist, stateSize);
			status = STATUS_SUCCESS;
			writeSize = stateSize;
		}
		else {
			KdPrint(("BoxDrv: BoxDrvReadList: Output buffer is too small!"));
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

	pInputBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (pInputBuffer) {
		bufInsize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
		ULONG stateSize = sizeof(stateInfo.watchlist);

		KdPrint(("BoxDrv: BoxDrvIoWriteList: In buffer size: %lu\r\n", bufInsize));
		KdPrint(("BoxDrv: BoxDrvIoWriteList: State size: %lu\r\n", stateSize));

		if (bufInsize <= stateSize) {
			RtlCopyMemory(stateInfo.watchlist, pInputBuffer, bufInsize);
			status = STATUS_SUCCESS;
			writeSize = bufInsize;
		}
		else {
			KdPrint(("BoxDrv: BoxDrvIoWriteList: Input buffer overflows state!"));
			status = STATUS_BUFFER_OVERFLOW;
		}
	}

	*dataWritten = writeSize;
	return status;
}
