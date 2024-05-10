#pragma once
#include <fltKernel.h>

// NT based name of the virtual driver device
#define NT_DEVICE_NAME		L"\\Device\\BoxDrv"
// DOS based name of the virtual driver device
#define DOS_DEViCE_NAME		L"\\DosDevices\\BoxDrv"

// Type used to track processes in watchlist
#define TRACK_TYPE			HANDLE

// Type definition for driver-wide state. `watchlist` is hardcoded to 32 elements,
// surely the user won't need to more than 32 applicatoins at once, right?
typedef struct {
	TRACK_TYPE watchlist[32];		// Holds the list of actively watched PIDs
	UINT32 regFlt;					// Internal testing flag for regFilter
	UINT32 fsFlt;					// Internal testing flag for fsFilter
} BoxDrvState, *PBoxDrvState;

// IOCTL codes for communication
// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/introduction-to-i-o-control-codes

#define BOXDRV_IO_ECHO			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define BOXDRV_IO_READLIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS)
#define BOXDRV_IO_WRITELIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_WRITE_ACCESS)
