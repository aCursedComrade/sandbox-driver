#include "registry.h"

#pragma alloc_text(INIT, BoxDrvInitRegFilter)
#pragma alloc_text(PAGE, BoxDrvRemoveRegFilter)
#pragma alloc_text(PAGE, BoxDrvRegCallbackHandler)
#pragma alloc_text(PAGE, BoxDrvRegPreOpenKeyFn)
#pragma alloc_text(PAGE, BoxDrvRegPreCreateKeyFn)
#pragma alloc_text(PAGE, BoxDrvRegKeyHandleCloseFn)

// Global vaiables

// A "cookie" given by configuration manager for registry based actions
LARGE_INTEGER cmCookie = { 0 };

// Holds the function pointers to different class-based callback functions
PEX_CALLBACK_FUNCTION registryCallBackTable[MaxRegNtNotifyClass] = { 0 };

// Registry filter definitions

// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/handling-notifications

NTSTATUS BoxDrvInitRegFilter(_In_ PDRIVER_OBJECT pDriverObject) {
	UNICODE_STRING altitude = RTL_CONSTANT_STRING(regFltAltitude);

	// populate the global callback table
	// we will only look at 3 events for the prototype:
	// - Pre-open key events
	// - Pre-create key events
	// - Key handle closing events
	registryCallBackTable[RegNtPreOpenKeyEx] = (PEX_CALLBACK_FUNCTION)BoxDrvRegPreOpenKeyFn;
	registryCallBackTable[RegNtPreCreateKey] = (PEX_CALLBACK_FUNCTION)BoxDrvRegPreCreateKeyFn;
	registryCallBackTable[RegNtKeyHandleClose] = (PEX_CALLBACK_FUNCTION)BoxDrvRegKeyHandleCloseFn;

	// register our global handler and return its status
	return CmRegisterCallbackEx(BoxDrvRegCallbackHandler, &altitude, pDriverObject, NULL, &cmCookie, NULL);
}

VOID BoxDrvRemoveRegFilter() {
	PAGED_CODE();

	// bad things *can* happen if this fails
	// this is only called driver unload though

	NTSTATUS status = CmUnRegisterCallback(cmCookie);
	if (!NT_SUCCESS(status)) {
		DbgPrint("BoxDrv: Failed to unregister our registry filter\r\n");
	}
}

NTSTATUS BoxDrvRegCallbackHandler(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PVOID callBackData) {
	PAGED_CODE();

	REG_NOTIFY_CLASS nClass = (REG_NOTIFY_CLASS)(ULONG_PTR)notifyClass;
	
	// return OK if the class operation is not included in the table
	if (!registryCallBackTable[nClass]) {
		return STATUS_SUCCESS;
	}

	return registryCallBackTable[nClass](callBackContext, notifyClass, callBackData);
}

NTSTATUS BoxDrvRegPreOpenKeyFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_OPEN_KEY_INFORMATION callBackData) {
	NTSTATUS status = STATUS_SUCCESS;
	PUNICODE_STRING pCompleteName = NULL;

	UNREFERENCED_PARAMETER(callBackContext);
	UNREFERENCED_PARAMETER(notifyClass);
	PAGED_CODE();

	// check if we can find the complete name for a given key
	if (callBackData->CompleteName->Length == 0 || *callBackData->CompleteName->Buffer != OBJ_NAME_PATH_SEPARATOR) {
		PCUNICODE_STRING pRootObjectName = NULL;
		status = CmCallbackGetKeyObjectIDEx(&cmCookie, callBackData->RootObject, NULL, &pRootObjectName, 0);

		if (NT_SUCCESS(status)) {
			USHORT cbBuffer = pRootObjectName->Length;
			cbBuffer += sizeof(wchar_t);
			cbBuffer += callBackData->CompleteName->Length;
			ULONG cbString = sizeof(UNICODE_STRING) + cbBuffer;

			pCompleteName = (PUNICODE_STRING)ExAllocatePool2(POOL_FLAG_PAGED, cbString, 'tsBC');
			if (pCompleteName) {
				pCompleteName->Length = 0;
				pCompleteName->MaximumLength = cbBuffer;
				pCompleteName->Buffer = (PWCH)((PCCH)pCompleteName + sizeof(UNICODE_STRING));

				RtlCopyUnicodeString(pCompleteName, pRootObjectName);
				RtlAppendUnicodeToString(pCompleteName, L"\\");
				RtlAppendUnicodeStringToString(pCompleteName, callBackData->CompleteName);
			}

			CmCallbackReleaseKeyObjectIDEx(pRootObjectName);
		}
		else {
			DbgPrint("BoxDrv: Could not get the complete name for the registry key\r\n");
		}
	}

	DbgPrint("BoxDev: PreOpenKey: Key name is %wZ\r\n", pCompleteName ? pCompleteName : callBackData->CompleteName);

	if (pCompleteName) {
		ExFreePool(pCompleteName);
	}

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvRegPreCreateKeyFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_CREATE_KEY_INFORMATION callBackData) {
	UNREFERENCED_PARAMETER(callBackContext);
	UNREFERENCED_PARAMETER(notifyClass);
	UNREFERENCED_PARAMETER(callBackData);
	PAGED_CODE();

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvRegKeyHandleCloseFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_KEY_HANDLE_CLOSE_INFORMATION callBackData) {
	UNREFERENCED_PARAMETER(callBackContext);
	UNREFERENCED_PARAMETER(notifyClass);
	UNREFERENCED_PARAMETER(callBackData);
	PAGED_CODE();

	return STATUS_SUCCESS;
}
