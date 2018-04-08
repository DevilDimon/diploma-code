#include "gene_ioctl.h"
#include "Public.h"
#include "..\gene_runtime.h"
#include "..\gene_embedded_types.h"

extern GeneRuntime gene_runtime;

VOID DrvEvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength,
	IN size_t InputBufferLength, IN ULONG IoControlCode)
{
	NTSTATUS status = STATUS_SUCCESS; // Assume success
	PCHAR inBuf = nullptr, outBuf = nullptr; // pointer to Input, Output buffers
	size_t bufSize;

	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(OutputBufferLength);

	PAGED_CODE();

	if (!InputBufferLength)
	{
		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
		return;
	}

	//
	// Determine which I/O control code was specified.
	//

	switch (IoControlCode)
	{
	case IOCTL_TEST_METHOD_BUFFERED: {

		//
		// For bufffered ioctls WdfRequestRetrieveInputBuffer &
		// WdfRequestRetrieveOutputBuffer return the same buffer
		// pointer (Irp->AssociatedIrp.SystemBuffer), so read the
		// content of the buffer before writing to it.
		//
		status = WdfRequestRetrieveInputBuffer(Request, 0, reinterpret_cast<PVOID *>(&inBuf), &bufSize);
		if (!NT_SUCCESS(status)) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
		ASSERT(bufSize == InputBufferLength);

		gene_internal::container in;
		for (int i = 0; i < bufSize; ++i) {
			in.push_back(inBuf[i]);
		}

		gene_internal::container out;
		gene_runtime.ProcessIncomingMessage(in, &out);

		status = WdfRequestRetrieveOutputBuffer(Request, 0, reinterpret_cast<PVOID *>(&outBuf), &bufSize);

		if (!NT_SUCCESS(status)) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
		ASSERT(bufSize == OutputBufferLength);

		RtlCopyMemory(outBuf, out.data(), out.size());

		WdfRequestSetInformation(Request,
			OutputBufferLength < out.size() ? OutputBufferLength : out.size());

		//
		// When the request is completed the content of the SystemBuffer
		// is copied to the User output buffer and the SystemBuffer is
		// is freed.
		//

		break;
	}

	default:

		//
		// The specified I/O control code is unrecognized by this driver.
		//
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	WdfRequestComplete(Request, status);
}