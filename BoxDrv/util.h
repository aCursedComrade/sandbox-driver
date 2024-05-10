#pragma once
#include "base.h"

// Checks if the pointed-to string buffer includes a null byte
BOOLEAN BoxDrvIsStringTerminated(_In_ PCHAR pString, _In_ UINT32 uLength);

// Checks if given PID exists in state list
BOOLEAN BoxDrvIsInWatchlist(_In_ TRACK_TYPE pid);
BOOLEAN BoxDrvAddToWatchlist(_In_ TRACK_TYPE pid);
VOID BoxDrvRemoveFromWatchlist(_In_ TRACK_TYPE pid);
