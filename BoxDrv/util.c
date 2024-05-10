#include "util.h"

#pragma alloc_text(PAGE, BoxDrvIsStringTerminated)
#pragma alloc_text(PAGE, BoxDrvIsInWatchlist)
#pragma alloc_text(PAGE, BoxDrvAddToWatchlist)
#pragma alloc_text(PAGE, BoxDrvRemoveFromWatchlist)

// Global variables

extern BoxDrvState stateInfo;

// Function definitions

BOOLEAN BoxDrvIsStringTerminated(_In_ PCHAR pString, _In_ UINT32 length) {
	BOOLEAN terminated = FALSE;
	UINT32 index = 0;

	PAGED_CODE();

	while (index < length && terminated == FALSE) {
		if (pString[index] == '\0') {
			terminated = TRUE;
		} else {
			index++;
		}
	}

	return terminated;
}

BOOLEAN BoxDrvIsInWatchlist(_In_ TRACK_TYPE pid) {
	UINT32 stateLen = sizeof(stateInfo) / sizeof(TRACK_TYPE);

	PAGED_CODE();

	for (UINT32 index = 0; index < stateLen; index++) {
		if (stateInfo.watchlist[index] == pid) return TRUE;
	}

	return FALSE;
}

BOOLEAN BoxDrvAddToWatchlist(_In_ TRACK_TYPE pid) {
	UINT32 stateLen = sizeof(stateInfo) / sizeof(TRACK_TYPE);
	
	PAGED_CODE();

	for (UINT32 index = 0; index < stateLen; index++) {
		if (stateInfo.watchlist[index] == 0) {
			stateInfo.watchlist[index] = pid;
			return TRUE;
		};
	}

	return FALSE;
}

VOID BoxDrvRemoveFromWatchlist(_In_ TRACK_TYPE pid) {
	UINT32 stateLen = sizeof(stateInfo) / sizeof(TRACK_TYPE);

	PAGED_CODE();

	for (UINT32 index = 0; index < stateLen; index++) {
		if (stateInfo.watchlist[index] == pid) {
			stateInfo.watchlist[index] = 0;
		};
	}
}
