#pragma once
#include <ntdef.h>

PVOID __cdecl operator new(size_t iSize);
void __cdecl operator delete(PVOID pVoid, size_t iSize);
