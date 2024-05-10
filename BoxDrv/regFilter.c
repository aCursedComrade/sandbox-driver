#include "regFilter.h"
#include "util.h"

#pragma alloc_text(INIT, BoxDrvRegFilterInit)
#pragma alloc_text(PAGE, BoxDrvRegFilterUnload)
#pragma alloc_text(PAGE, BoxDrvRegCallbackHandler)
#pragma alloc_text(TEXT, BoxDrvRegPreOpenOrCreateKeyFn)
#pragma alloc_text(TEXT, BoxDrvRegKeyHandleCloseFn)

// Global variables

extern BoxDrvState stateInfo;

// A "cookie" given by configuration manager for registry based actions
LARGE_INTEGER cmCookie = { 0 };

// Holds the function pointers to different class-based callback functions
PEX_CALLBACK_FUNCTION registryCallBackTable[MaxRegNtNotifyClass] = { 0 };

// Registry filter definitions
// - Pre* events typically sends a part of registry key, so we need to manually resolve the abosolute path (with CmCallbackGetKeyObjectIDEx)
// - CmCallbackGetKeyObjectIDEx must be followed by CmCallbackReleaseKeyObjectIDEx appropiately
// - https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/handling-notifications

// Function definitions

NTSTATUS BoxDrvRegFilterInit(_In_ PDRIVER_OBJECT pDriverObject) {
	UNICODE_STRING altitude = RTL_CONSTANT_STRING(regFltAltitude);

	// populate the global callback table
	// we will only look at 3 events for the prototype:
	// - Pre-open key events *
	// - Pre-create key events *
	// - Key handle closing events
	// * Both have the same event data types
	registryCallBackTable[RegNtPreOpenKeyEx] = (PEX_CALLBACK_FUNCTION)BoxDrvRegPreOpenOrCreateKeyFn;
	registryCallBackTable[RegNtPreCreateKeyEx] = (PEX_CALLBACK_FUNCTION)BoxDrvRegPreOpenOrCreateKeyFn;
	registryCallBackTable[RegNtKeyHandleClose] = (PEX_CALLBACK_FUNCTION)BoxDrvRegKeyHandleCloseFn;

	// register our global handler and return its status
	return CmRegisterCallbackEx(BoxDrvRegCallbackHandler, &altitude, pDriverObject, NULL, &cmCookie, NULL);
}

VOID BoxDrvRegFilterUnload() {
	PAGED_CODE();

	// bad things *can* happen if this fails
	// this is only called driver unload though

	NTSTATUS status = CmUnRegisterCallback(cmCookie);
	if (!NT_SUCCESS(status)) {
		KdPrint(("BoxDrv: Failed to unregister our registry filter\r\n"));
	}
}

NTSTATUS BoxDrvRegCallbackHandler(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PVOID callBackData) {
	PAGED_CODE();

	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-ex_callback_function

	REG_NOTIFY_CLASS nClass = (REG_NOTIFY_CLASS)(ULONG_PTR)notifyClass;
	
	// return OK if the class operation is not included in the table
	if (!registryCallBackTable[nClass]) {
		return STATUS_SUCCESS;
	}

	return registryCallBackTable[nClass](callBackContext, notifyClass, callBackData);
}

NTSTATUS BoxDrvRegPreOpenOrCreateKeyFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_CREATE_KEY_INFORMATION_V1 callBackData) {
	UNREFERENCED_PARAMETER(callBackContext);
	UNREFERENCED_PARAMETER(notifyClass);

	PUNICODE_STRING pCompleteName = NULL;
	HANDLE pProcess = PsGetCurrentProcessId();

	// let the request go through if it is not watchlist
	if (!BoxDrvIsInWatchlist(pProcess)) return STATUS_SUCCESS;

	// check if we can find the complete name for a given key if its a relative name
	if (callBackData->CompleteName->Length == 0 || *callBackData->CompleteName->Buffer != OBJ_NAME_PATH_SEPARATOR) {
		NTSTATUS status = STATUS_SUCCESS;
		PCUNICODE_STRING pRootObjectName = NULL;

		// attempt to obtain the name associated with the root object/context
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
			KdPrint(("BoxDrv: PreOpenOrCreateKey: Failed to resolve the root object name\r\n"));
		}
	}

	// actual preprocessing is done here, like deciding if we want to allow or deny access
	// we will choose to let the requests pass-through when we fail to resolve the complete path
	if (pCompleteName) {
		KdPrint(("BoxDev: PreOpenOrCreateKey: Key accessed (by PID: %llu: %wZ\r\n", (ULONG_PTR)pProcess, pCompleteName));
		ExFreePool(pCompleteName);
	}

	return STATUS_SUCCESS;
}

NTSTATUS BoxDrvRegKeyHandleCloseFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_KEY_HANDLE_CLOSE_INFORMATION callBackData) {
	UNREFERENCED_PARAMETER(callBackContext);
	UNREFERENCED_PARAMETER(notifyClass);

	PCUNICODE_STRING pObjectName = NULL;
	HANDLE pProcess = PsGetCurrentProcessId();

	// let the request go through if it is not watchlist
	if (!BoxDrvIsInWatchlist(pProcess)) return STATUS_SUCCESS;

	// attempt to obtain the registry key name associated with the object
	NTSTATUS status = STATUS_SUCCESS;
	status = CmCallbackGetKeyObjectIDEx(&cmCookie, callBackData->Object, NULL, &pObjectName, 0);

	if (NT_SUCCESS(status)) {
		KdPrint(("BoxDev: KeyHandleCloseFn: Closing handle for key (by PID: %llu): %wZ\r\n", (ULONG_PTR)pProcess, pObjectName));
	}
	else {
		KdPrint(("BoxDev: KeyHandleCloseFn: Failed to resolve the object name\r\n"));
	}

	CmCallbackReleaseKeyObjectIDEx(pObjectName);

	return STATUS_SUCCESS;
}
