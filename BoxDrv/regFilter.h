#pragma once
#include "base.h"

// Altitude at which the registry filter is placed
#define regFltAltitude			L"36000"

// Initialize the registry filter
NTSTATUS BoxDrvRegInitFilter(_In_ PDRIVER_OBJECT pDriverObject);

// Unregister the registry filter
VOID BoxDrvRegRemoveFilter();

// Main registry event callback handler
NTSTATUS BoxDrvRegCallbackHandler(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PVOID callBackData);

// Class-based registry event callback handler
NTSTATUS BoxDrvRegPreOpenOrCreateKeyFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_CREATE_KEY_INFORMATION_V1 callBackData);
NTSTATUS BoxDrvRegKeyHandleCloseFn(_In_ PVOID callBackContext, _In_ PVOID notifyClass, _In_ PREG_KEY_HANDLE_CLOSE_INFORMATION callBackData);
