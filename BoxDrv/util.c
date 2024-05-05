#include "util.h"

#pragma alloc_text(PAGE, IsStringTerminated)

BOOLEAN IsStringTerminated(_In_ char* pString, _In_ unsigned int length) {
	BOOLEAN terminated = FALSE;
	unsigned int index = 0;

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
