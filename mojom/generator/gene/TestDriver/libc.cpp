#include "libc.h"
#include <ntddk.h>

#define POOL_TAG 'wNCK'

PVOID operator new(size_t iSize) {
  PVOID result = ExAllocatePoolWithTag(NonPagedPool, iSize, POOL_TAG);
  if (result) {
    RtlZeroMemory(result, iSize);
  }
  return result;
}

void __cdecl operator delete(PVOID pVoid, size_t) {
  if (pVoid) {
    ExFreePool(pVoid);
  }
}

extern "C"
{
  typedef void __cdecl vfv_t(void);
  typedef int  __cdecl ifv_t(void);
  typedef void __cdecl vfi_t(int);
}

#pragma region section_attributes
#pragma comment(linker, "/merge:.CRT=.rdata")

extern "C" int _cdecl atexit(vfv_t) { return 0; }
/*
extern "C" int _cdecl at_quick_exit(void *f) { return 0; }

extern "C" int _cdecl _purecall(void) { return 0; }

void __cdecl exit(int status) {}

void __cdecl quick_exit(int status) {}*/
