#pragma once
#include "base.h"

// Checks if the pointed-to string buffer includes a null byte
BOOLEAN IsStringTerminated(_In_ char* pString, _In_ unsigned int uLength);
