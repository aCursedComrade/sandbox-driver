#pragma once
#include "base.h"

// Driver entrypoint
DRIVER_INITIALIZE DriverEntry;
// Driver unload function
DRIVER_UNLOAD BoxDrvUnload;

// IRP major functions
NTSTATUS BoxDrvCreateClose(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
NTSTATUS BoxDrvCleanup(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
NTSTATUS BoxDrvIoControl(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
NTSTATUS BoxDrvRead(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
NTSTATUS BoxDrvWrite(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
NTSTATUS BoxDrvUnsupported(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp);
