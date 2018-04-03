#include <ntddk.h>
#include <wdf.h>
#include <wdmsec.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include "Public.h"
#include "KVector.h"
#include "..\gene_embedded_types.h"
#include "gene_ioctl.h"

#include "..\gene_runtime.h"
#include "..\test.mojom.h"

extern "C" DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_UNLOAD TestEvtDriverUnload;
EVT_WDF_DEVICE_SHUTDOWN_NOTIFICATION TestShutdown;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL DrvEvtIoDeviceControl;

extern "C" NTSTATUS TestDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit);

GeneRuntime gene_runtime;

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
	// Call TestDeviceAdd to create a deviceobject to represent our
	// software device.
	//
	status = TestDeviceAdd(hDriver, pInit);

	class CoreImpl : public Core {
		bool Init(const MyStruct &a, const MySecondStruct &b) override {
			UNREFERENCED_PARAMETER(a);
			UNREFERENCED_PARAMETER(b);
			DbgBreakPoint();
			return true;
		}

		bool Shutdown(const MyStruct &a, const MySecondStruct &b) override {
			UNREFERENCED_PARAMETER(a);
			UNREFERENCED_PARAMETER(b);
			DbgBreakPoint();
			return true;
		}
	};

	gene_runtime = GeneRuntime();
	gene_runtime.RegisterCoreHandler(new CoreImpl());

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

	ioQueueConfig.EvtIoDeviceControl = DrvEvtIoDeviceControl;

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
