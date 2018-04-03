#pragma once
#include <ntddk.h>
#include <wdf.h>

VOID DrvEvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength,
	IN size_t InputBufferLength, IN ULONG IoControlCode);