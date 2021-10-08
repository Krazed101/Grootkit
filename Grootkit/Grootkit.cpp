#include <ntddk.h>

// prototypes
void GrootkitUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS GrootkitCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);

//Driver Entry
extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DriverObject->DriverUnload = GrootkitUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = GrootkitCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = GrootkitCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = GrootkitDeviceControl;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Grootkit");

	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
	);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create device object (0x%08X)\n", status));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Grootkit");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create symbolic link (0x%08X)\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}

void GrootkitUnload(PDRIVER_OBJECT DriverObject) {

}