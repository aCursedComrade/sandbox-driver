#pragma once
#include "base.h"

typedef NTSTATUS(*QUERY_INFO_PROCESS) (
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
);

NTSTATUS BoxDrvProcRegister();
VOID BoxDrvProcUnload();

VOID BoxDrvProcMonitor(_In_ HANDLE ppid, _In_ HANDLE pid, _In_ BOOLEAN create);
NTSTATUS BoxDrvProcGetName(_In_ PEPROCESS pProcess, _Out_ PUNICODE_STRING* pProcessImageName);
