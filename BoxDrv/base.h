#pragma once
#include <ntddk.h>
#include <wdm.h>

// NT based name of the virtual driver device
#define NT_DEVICE_NAME		L"\\Device\\BoxDrv"
// DOS based name of the virtual driver device
#define DOS_DEViCE_NAME		L"\\DosDevices\\BoxDrv"
