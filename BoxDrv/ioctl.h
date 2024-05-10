#pragma once
#include "base.h"

NTSTATUS BoxDrvIoEcho(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten);
NTSTATUS BoxDrvIoReadList(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten);
NTSTATUS BoxDrvIoWriteList(_In_ PIRP Irp, _In_ PIO_STACK_LOCATION pIrpStack, _Out_ UINT32* dataWritten);
