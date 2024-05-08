#include "driver.h"
#include "util.h"
#include "regFilter.h"
#include "fsFilter.h"

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, BoxDrvUnload)
#pragma alloc_text(PAGE, BoxDrvCreateClose)
#pragma alloc_text(PAGE, BoxDrvCleanup)
#pragma alloc_text(PAGE, BoxDrvIoControl)
#pragma alloc_text(PAGE, BoxDrvRead)
#pragma alloc_text(PAGE, BoxDrvWrite)

// Global vaiables

// Core driver definitions

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath) {
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING ntDevice, dosDevice;

	KdPrint(("BoxDrv: DriverEntry invoked\r\n"));

	// Register our registry filter
	status = BoxDrvRegInitFilter(pDriverObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to register our registry filter\r\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

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
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = BoxDrvCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = BoxDrvCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = BoxDrvCleanup;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BoxDrvIoControl;
	pDriverObject->MajorFunction[IRP_MJ_READ] = BoxDrvRead;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = BoxDrvWrite;
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

	return STATUS_SUCCESS;
}

VOID BoxDrvUnload(_In_ PDRIVER_OBJECT pDriverObject) {
	PAGED_CODE();

	PDEVICE_OBJECT pDeviceObject = pDriverObject->DeviceObject;
	UNICODE_STRING dosDevice;

	BoxDrvRegRemoveFilter();

	RtlInitUnicodeString(&dosDevice, DOS_DEViCE_NAME);

	// tear down virtual device in when unloading
	IoDeleteSymbolicLink(&dosDevice);
	if (pDeviceObject != NULL) {
		IoDeleteDevice(pDeviceObject);
	}

	KdPrint(("BoxDrv: Driver unloaded\r\n"));
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

NTSTATUS BoxDrvCleanup(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IRP_MJ_CLEANUP completed\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvIoControl(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IOCTL request complete\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvRead(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: IRP_MJ_READ completed\r\n"));

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvWrite(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION pIrpStack = NULL;
	char* writeBuffer = NULL;
	ULONG bufferLen = 0;

	KdPrint(("BoxDrv: ==> IRP_MJ_WRITE\r\n"));

	pIrpStack = IoGetCurrentIrpStackLocation(Irp);

	if (pIrpStack) {
		writeBuffer = (char*)Irp->AssociatedIrp.SystemBuffer;

		if (writeBuffer) {
			bufferLen = pIrpStack->Parameters.Write.Length;
			KdPrint(("BoxDrv: Buffer length: %lu\r\n", bufferLen));

			if (IsStringTerminated(writeBuffer, bufferLen)) {
				KdPrint(("BoxDrv: Buffer: %s\r\n", writeBuffer));
			}
		}
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = bufferLen;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("BoxDrv: <== IRP_MJ_WRITE\r\n"));

	return status;
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
