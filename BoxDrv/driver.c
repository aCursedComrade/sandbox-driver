#include "driver.h"
#include "util.h"

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, BoxDrvUnload)
#pragma alloc_text(PAGE, BoxDrvCreate)
#pragma alloc_text(PAGE, BoxDrvClose)
#pragma alloc_text(PAGE, BoxDrvIoControl)
#pragma alloc_text(PAGE, BoxDrvRead)
#pragma alloc_text(PAGE, BoxDrvWrite)

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath) {
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING ntDevice, dosDevice;

	DbgPrint("BoxDrv: DriverEntry\r\n");

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
		DbgPrint("BoxDrv: Failed to create device");
		return status;
	}

	// setup the driver functions
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = BoxDrvCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = BoxDrvClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BoxDrvIoControl;
	pDriverObject->MajorFunction[IRP_MJ_READ] = BoxDrvRead;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = BoxDrvWrite;
	pDriverObject->DriverUnload = BoxDrvUnload;

	// driver flags
	// - we set preferred mode of IO
	pDriverObject->Flags |= DO_BUFFERED_IO;

	// link DOS and NT device names
	status = IoCreateSymbolicLink(&dosDevice, &ntDevice);

	if (!NT_SUCCESS(status)) {
		DbgPrint("BoxDrv: Failed to create symbolic link");
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	return status;
}

VOID BoxDrvUnload(_In_ PDRIVER_OBJECT pDriverObject) {
	PDEVICE_OBJECT pDeviceObject = pDriverObject->DeviceObject;
	UNICODE_STRING dosDevice;

	PAGED_CODE();

	RtlInitUnicodeString(&dosDevice, DOS_DEViCE_NAME);

	// tear down virtual device in when unloading
	IoDeleteSymbolicLink(&dosDevice);
	if (pDeviceObject != NULL) {
		IoDeleteDevice(pDeviceObject);
	}

	DbgPrint("BoxDrv: Driver unloaded\r\n");
}

NTSTATUS BoxDrvCreate(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("BoxDrv: Create function called\r\n");

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvClose(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("BoxDrv: Close function called\r\n");

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvIoControl(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvRead(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvWrite(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP Irp) {
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION pIrpStack = NULL;
	char* writeBuffer = NULL;

	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	DbgPrint("BoxDrv: Write function called\r\n");

	pIrpStack = IoGetCurrentIrpStackLocation(Irp);

	if (pIrpStack) {
		writeBuffer = (char*)Irp->AssociatedIrp.SystemBuffer;

		if (writeBuffer) {
			if (IsStringTerminated(writeBuffer, pIrpStack->Parameters.Write.Length)) {
				DbgPrint("BoxDrv: %s", writeBuffer);
			}
		}
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}
