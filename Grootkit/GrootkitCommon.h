#pragma once
#include<ntddk.h>

#define GROOTKIT_DEVICE 0x8000

#define IOCTL_GROOTKIT CTL_CODE(GROOTKIT_DEVICE,0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

struct FileData {
	UNICODE_STRING filePath;
};
