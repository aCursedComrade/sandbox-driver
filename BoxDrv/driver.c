#include "driver.h"
#include "util.h"
#include "proc.h"
#include "regFilter.h"
#include "ioctl.h"

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, BoxDrvUnload)
#pragma alloc_text(PAGE, BoxDrvIoControl)
#pragma alloc_text(PAGE, BoxDrvCreateClose)
#pragma alloc_text(PAGE, BoxDrvReadWrite)
#pragma alloc_text(PAGE, BoxDrvCleanup)
#pragma alloc_text(PAGE, BoxDrvUnsupported)

// Driver-wide variables

BoxDrvState stateInfo = {
	{ (HANDLE)69420 },
	0,
	0,
};

// Core driver definitions

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath) {
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING ntDevice, dosDevice;

	KdPrint(("BoxDrv: DriverEntry invoked\r\n"));

	RtlInitUnicodeString(&ntDevice, NT_DEVICE_NAME);
	RtlInitUnicodeString(&dosDevice, DOS_DEViCE_NAME);
	
	// create the virtual device used to talk to the driver
	status = IoCreateDevice(
		pDriverObject,
		0,
		&ntDevice,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&pDeviceObject
	);

	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to create device\r\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// initiate all major functions to unsupported
	for (UINT32 findex = 0; findex < IRP_MJ_MAXIMUM_FUNCTION; findex++) {
		pDriverObject->MajorFunction[findex] = BoxDrvUnsupported;
	}

	// setup the supported driver functions
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BoxDrvIoControl;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = BoxDrvCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = BoxDrvCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_READ] = BoxDrvReadWrite;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = BoxDrvReadWrite;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = BoxDrvCleanup;
	pDriverObject->DriverUnload = BoxDrvUnload;

	// device flags
	// - we set preferred mode of IO (Buffered IO)
	pDeviceObject->Flags |= DO_BUFFERED_IO;

	// link DOS and NT device names
	status = IoCreateSymbolicLink(&dosDevice, &ntDevice);
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to create device symbolic link\r\n"));
		IoDeleteDevice(pDeviceObject);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// Register process for process spawn events
	status = BoxDrvProcRegister();
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to register our process monitor\r\n"));
		IoDeleteDevice(pDeviceObject);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// Register our registry filter
	status = BoxDrvRegFilterInit(pDriverObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to register our registry filter\r\n"));
		IoDeleteDevice(pDeviceObject);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	return STATUS_SUCCESS;
}

VOID BoxDrvUnload(_In_ PDRIVER_OBJECT pDriverObject) {
	PDEVICE_OBJECT pDeviceObject = pDriverObject->DeviceObject;
	UNICODE_STRING dosDevice;

	PAGED_CODE();

	// Unregister our process monitor
	BoxDrvProcUnload();

	// Unregister our registry filter
	BoxDrvRegFilterUnload();

	RtlInitUnicodeString(&dosDevice, DOS_DEViCE_NAME);

	// Tear down devices
	IoDeleteSymbolicLink(&dosDevice);
	if (pDeviceObject != NULL) {
		IoDeleteDevice(pDeviceObject);
	}

	KdPrint(("BoxDrv: Driver unloaded\r\n"));
}

NTSTATUS BoxDrvIoControl(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION pIrpStack = NULL;
	UINT32 dataWritten = 0;

	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	KdPrint(("BoxDrv: ===> IOCTL event\r\n"));
	pIrpStack = IoGetCurrentIrpStackLocation(Irp);

	// we handle IO control requests here
	// controls we support are defined in <base.h> as BOXDRV_IO_* macros

	if (pIrpStack) {
		switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
		{
		case BOXDRV_IO_ECHO:
			status = BoxDrvIoEcho(Irp, pIrpStack, &dataWritten);
			break;
		case BOXDRV_IO_READLIST:
			status = BoxDrvIoReadList(Irp, pIrpStack, &dataWritten);
			break;
		case BOXDRV_IO_WRITELIST:
			status = BoxDrvIoWriteList(Irp, pIrpStack, &dataWritten);
			break;
		default:
			KdPrint(("BoxDrv: Unknown IOCTL request received with control code: %lu", pIrpStack->Parameters.DeviceIoControl.IoControlCode));
			break;
		}
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = dataWritten;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: <=== IOCTL request\r\n"));

	return status;
}

NTSTATUS BoxDrvCreateClose(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IRP_MJ_CREATE/CLOSE completed\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvReadWrite(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IRP_MJ_READ/WRITE complete\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvCleanup(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IRP_MJ_CLEANUP completed\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvUnsupported(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	KdPrint(("BoxDrv: Unsupported IRP_MJ_* called\r\n"));

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_NOT_SUPPORTED;
}
