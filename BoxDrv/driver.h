#pragma once
#include "base.h"

// Driver entrypoint
DRIVER_INITIALIZE DriverEntry;
// Driver unload function
DRIVER_UNLOAD BoxDrvUnload;

// IRP major functions
_Dispatch_type_(IRP_MJ_CREATE) DRIVER_DISPATCH BoxDrvCreate;
_Dispatch_type_(IRP_MJ_CLOSE) DRIVER_DISPATCH BoxDrvClose;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH BoxDrvIoControl;
_Dispatch_type_(IRP_MJ_READ) DRIVER_DISPATCH BoxDrvRead;
_Dispatch_type_(IRP_MJ_WRITE) DRIVER_DISPATCH BoxDrvWrite;

// Registry callback functions
//NTSTATUS BoxDrvRegistryCallback(_In_ PVOID CallbackContext, _In_ PVOID Arg1, _In_ PVOID Arg2);
