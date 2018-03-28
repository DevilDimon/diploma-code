#include <ntddk.h>
#include <wdf.h>
#include <wdmsec.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include "Public.h"
#include "KVector.h"
#include "..\gene_embedded_types.h"

#include "..\test.mojom.h"

extern "C" DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_UNLOAD TestEvtDriverUnload;
EVT_WDF_DEVICE_SHUTDOWN_NOTIFICATION TestShutdown;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FileEvtIoDeviceControl;

extern "C" NTSTATUS TestDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit);
VOID PrintChars(_In_reads_(CountChars) PCHAR BufferAddress, _In_ size_t CountChars);

NTSTATUS DriverEntry(_DRIVER_OBJECT* DriverObject, PUNICODE_STRING RegistryPath) 
{
	NTSTATUS status;
	WDF_DRIVER_CONFIG config;
	WDFDRIVER hDriver;
	PWDFDEVICE_INIT pInit = nullptr;

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Hello World!\n"));
	WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);
	//
	// Tell the framework that this is non-pnp driver so that it doesn't
	// set the default AddDevice routine.
	//
	config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

	//
	// NonPnp driver must explicitly register an unload routine for
	// the driver to be unloaded.
	//
	config.EvtDriverUnload = TestEvtDriverUnload;

	//
	// Create a framework driver object to represent our driver.
	//
	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		WDF_NO_OBJECT_ATTRIBUTES,
		&config,
		&hDriver);
	if (!NT_SUCCESS(status)) {
		KdPrint(("TestDriver: WdfDriverCreate failed with status 0x%x\n", status));
		return status;
	}

	//
	//
	// In order to create a control device, we first need to allocate a
	// WDFDEVICE_INIT structure and set all properties.
	//
	pInit = WdfControlDeviceInitAllocate(
		hDriver,
		&SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R
	);

	if (pInit == nullptr) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		return status;
	}

	//
	// Call NonPnpDeviceAdd to create a deviceobject to represent our
	// software device.
	//
	status = TestDeviceAdd(hDriver, pInit);

	return status;
}

NTSTATUS TestDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
/*++

Routine Description:

Called by the DriverEntry to create a control-device. This call is
responsible for freeing the memory for DeviceInit.

Arguments:

DriverObject - a pointer to the object that represents this device
driver.

DeviceInit - Pointer to a driver-allocated WDFDEVICE_INIT structure.

Return Value:

STATUS_SUCCESS if initialized; an error otherwise.

--*/
{
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_IO_QUEUE_CONFIG ioQueueConfig;
	WDFQUEUE queue;
	WDFDEVICE controlDevice;
	DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING);
	DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING);

	UNREFERENCED_PARAMETER(Driver);

	PAGED_CODE();

	//
	// Set exclusive to TRUE so that no more than one app can talk to the
	// control device at any time.
	//
	WdfDeviceInitSetExclusive(DeviceInit, TRUE);

	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);


	status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);

	if (!NT_SUCCESS(status)) {
		goto End;
	}

	WdfControlDeviceInitSetShutdownNotification(DeviceInit,
		TestShutdown,
		WdfDeviceShutdown);

	status = WdfDeviceCreate(&DeviceInit,
		WDF_NO_OBJECT_ATTRIBUTES,
		&controlDevice);
	if (!NT_SUCCESS(status)) {
		goto End;
	}

	//
	// Create a symbolic link for the control object so that usermode can open
	// the device.
	//


	status = WdfDeviceCreateSymbolicLink(controlDevice,
		&symbolicLinkName);

	if (!NT_SUCCESS(status)) {
		//
		// Control device will be deleted automatically by the framework.
		//
		goto End;
	}

	//
	// Configure a default queue so that requests that are not
	// configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
	// other queues get dispatched here.
	//
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
		WdfIoQueueDispatchSequential);

	ioQueueConfig.EvtIoDeviceControl = FileEvtIoDeviceControl;

	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	//
	// Since we are using Zw function set execution level to passive so that
	// framework ensures that our Io callbacks called at only passive-level
	// even if the request came in at DISPATCH_LEVEL from another driver.
	//
	//attributes.ExecutionLevel = WdfExecutionLevelPassive;

	//
	// By default, Static Driver Verifier (SDV) displays a warning if it 
	// doesn't find the EvtIoStop callback on a power-managed queue. 
	// The 'assume' below causes SDV to suppress this warning. If the driver 
	// has not explicitly set PowerManaged to WdfFalse, the framework creates
	// power-managed queues when the device is not a filter driver.  Normally 
	// the EvtIoStop is required for power-managed queues, but for this driver
	// it is not needed b/c the driver doesn't hold on to the requests or 
	// forward them to other drivers. This driver completes the requests 
	// directly in the queue's handlers. If the EvtIoStop callback is not 
	// implemented, the framework waits for all driver-owned requests to be
	// done before moving in the Dx/sleep states or before removing the 
	// device, which is the correct behavior for this type of driver.
	// If the requests were taking an indeterminate amount of time to complete,
	// or if the driver forwarded the requests to a lower driver/another stack,
	// the queue should have an EvtIoStop/EvtIoResume.
	//
	__analysis_assume(ioQueueConfig.EvtIoStop != 0);
	status = WdfIoQueueCreate(controlDevice,
		&ioQueueConfig,
		&attributes,
		&queue // pointer to default queue
	);
	__analysis_assume(ioQueueConfig.EvtIoStop == 0);
	if (!NT_SUCCESS(status)) {
		goto End;
	}

	//
	// Control devices must notify WDF when they are done initializing.   I/O is
	// rejected until this call is made.
	//
	WdfControlFinishInitializing(controlDevice);

End:
	//
	// If the device is created successfully, framework would clear the
	// DeviceInit value. Otherwise device create must have failed so we
	// should free the memory ourself.
	//
	if (DeviceInit != nullptr) {
		WdfDeviceInitFree(DeviceInit);
	}

	return status;

}

VOID FileEvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength,
	IN size_t InputBufferLength, IN ULONG IoControlCode)
/*++
Routine Description:

This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
requests from the system.

Arguments:

Queue - Handle to the framework queue object that is associated
with the I/O request.
Request - Handle to a framework request object.

OutputBufferLength - length of the request's output buffer,
if an output buffer is available.
InputBufferLength - length of the request's input buffer,
if an input buffer is available.

IoControlCode - the driver-defined or system-defined I/O control code
(IOCTL) that is associated with the request.

Return Value:

VOID

--*/
{
	NTSTATUS status = STATUS_SUCCESS; // Assume success
	PCHAR inBuf = nullptr; // pointer to Input buffer
	PCHAR data = "this String is from Device Driver !!!";
	ULONG datalen = (ULONG)strlen(data) + 1; //Length of data including null
	PCHAR buffer = nullptr;
	size_t bufSize;

	UNREFERENCED_PARAMETER(Queue);

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

		std::vector<uint8_t> c;
		for (int i = 0; i < bufSize; ++i) {
			c.push_back(inBuf[i]);
		}
		
		uint64_t service_id;
		if (!gene_internal::deserialize(c, &service_id)) DbgBreakPoint();
		uint64_t method_id;
		if (!gene_internal::deserialize(c, &method_id)) DbgBreakPoint();
		my_module::MyStruct a;
		if (!gene_internal::deserialize(c, &a)) DbgBreakPoint();
		my_module::MySecondStruct b;
		DbgBreakPoint();
		if (!gene_internal::deserialize(c, &b)) DbgBreakPoint();

		DbgBreakPoint();
		

		ASSERT(bufSize == InputBufferLength);

		//
		// Read the input buffer content.
		// We are using the following function to print characters instead
		// TraceEvents with %s format because the string we get may or
		// may not be null terminated. The buffer may contain non-printable
		// characters also.
		//
		PrintChars(inBuf, InputBufferLength);

		WdfRequestSetInformation(Request, 0);

		//
		// When the request is completed the content of the SystemBuffer
		// is copied to the User output buffer and the SystemBuffer is
		// is freed.
		//

		break;
	}


	case IOCTL_TEST_METHOD_IN_DIRECT:

		//
		// Get the Input buffer. WdfRequestRetrieveInputBuffer returns
		// Irp->AssociatedIrp.SystemBuffer.
		//
		status = WdfRequestRetrieveInputBuffer(Request, 0, reinterpret_cast<PVOID *>(&inBuf), &bufSize);
		if (!NT_SUCCESS(status)) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		ASSERT(bufSize == InputBufferLength);

		PrintChars(inBuf, InputBufferLength);

		//
		// Get the output buffer. Framework calls MmGetSystemAddressForMdlSafe
		// on the Irp->MdlAddress and returns the system address.
		// Oddity: For this method, this buffer is intended for transfering data
		// from the application to the driver.
		//

		status = WdfRequestRetrieveOutputBuffer(Request, 0, reinterpret_cast<PVOID *>(&buffer), &bufSize);
		if (!NT_SUCCESS(status)) {
			break;
		}

		ASSERT(bufSize == OutputBufferLength);

		PrintChars(buffer, OutputBufferLength);

		//
		// Return total bytes read from the output buffer.
		// Note OutputBufferLength = MmGetMdlByteCount(Irp->MdlAddress)
		//

		WdfRequestSetInformation(Request, OutputBufferLength);

		//
		// NOTE: Changes made to the  SystemBuffer are not copied
		// to the user input buffer by the I/O manager
		//

		break;

	case IOCTL_TEST_METHOD_OUT_DIRECT:

		//
		// Get the Input buffer. WdfRequestRetrieveInputBuffer returns
		// Irp->AssociatedIrp.SystemBuffer.
		//
		status = WdfRequestRetrieveInputBuffer(Request, 0, reinterpret_cast<PVOID *>(&inBuf), &bufSize);
		if (!NT_SUCCESS(status)) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		ASSERT(bufSize == InputBufferLength);

		PrintChars(inBuf, InputBufferLength);

		//
		// Get the output buffer. Framework calls MmGetSystemAddressForMdlSafe
		// on the Irp->MdlAddress and returns the system address.
		// For this method, this buffer is intended for transfering data from the
		// driver to the application.
		//
		status = WdfRequestRetrieveOutputBuffer(Request, 0, reinterpret_cast<PVOID *>(&buffer), &bufSize);
		if (!NT_SUCCESS(status)) {
			break;
		}

		ASSERT(bufSize == OutputBufferLength);

		//
		// Write data to be sent to the user in this buffer
		//
		RtlCopyMemory(buffer, data, OutputBufferLength);

		PrintChars(buffer, datalen);

		WdfRequestSetInformation(Request,
			OutputBufferLength < datalen ? OutputBufferLength : datalen);

		//
		// NOTE: Changes made to the  SystemBuffer are not copied
		// to the user input buffer by the I/O manager
		//

		break;
	default:

		//
		// The specified I/O control code is unrecognized by this driver.
		//
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	WdfRequestComplete(Request, status);
}

VOID TestShutdown(WDFDEVICE Device)
/*++

Routine Description:
Callback invoked when the machine is shutting down.  If you register for
a last chance shutdown notification you cannot do the following:
o Call any pageable routines
o Access pageable memory
o Perform any file I/O operations

If you register for a normal shutdown notification, all of these are
available to you.

This function implementation does nothing, but if you had any outstanding
file handles open, this is where you would close them.

Arguments:
Device - The device which registered the notification during init

Return Value:
None

--*/

{
	UNREFERENCED_PARAMETER(Device);
	return;
}


VOID TestEvtDriverUnload(IN WDFDRIVER Driver)
/*++
Routine Description:

Called by the I/O subsystem just before unloading the driver.
You can free the resources created in the DriverEntry either
in this routine or in the EvtDriverContextCleanup callback.

Arguments:

Driver - Handle to a framework driver object created in DriverEntry

Return Value:

NTSTATUS

--*/
{
	UNREFERENCED_PARAMETER(Driver);

	PAGED_CODE();

	return;
}

VOID PrintChars(_In_reads_(CountChars) PCHAR BufferAddress, _In_ size_t CountChars)
{
	if (CountChars) {

		while (CountChars--) {

			if (*BufferAddress > 31
				&& *BufferAddress != 127) {

				KdPrint(("%c", *BufferAddress));

			}
			else {

				KdPrint(("."));

			}
			BufferAddress++;
		}
		KdPrint(("\n"));
	}
	return;
}


