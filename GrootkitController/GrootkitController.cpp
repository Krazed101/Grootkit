// GrootkitController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "..\Grootkit\GrootkitCommon.h"
#include <subauth.h>

CommandType ParseCommand(std::string cmd);
int FileCreationCommandCreation(GrootkitData * data, char* argv[]);
void PrintHelp();
int Error(const char* message);

int main(int argc, const char* argv[]) {

	CommandType command_type;
	HANDLE hDevice;
	GrootkitData data;

	if (argc < 2) {
		PrintHelp();
		return 0;
	}
	
	std::string cmd = argv[1];
	command_type = ParseCommand(cmd);
	if (command_type == InvalidCommandType) {
		return 0;
	}
	hDevice = CreateFile(L"\\\\.\\Grootkit", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return Error("Failed to open device");
	}
	data.whichCommand = command_type;
	switch (command_type) {
		case FileCreation:

			break;
		default:
			PrintHelp();
			break;
	}

	return 0;
}

CommandType ParseCommand(std::string cmd) {
	
	for (char& c : cmd) {
		c = std::tolower(c);
	}

	if (!cmd.compare("help")) {
		printf("Commands\n-------------");
		printf("CreateFile\n");
		printf("\nFor command specifics type help after the command");
	}
	else if (!cmd.compare("createfile")) {
		return FileCreation;
	}

	return InvalidCommandType;
}

int FileCreationCommandCreation(GrootkitData * data, char* argv[]) {
	FileCreationData* fileCreation = &(data->command.FileCreation);
	std::string path = argv[2];
	if (!path.compare("help")) {
		printf("GrootkitController CreateFile <path> <data>\n");
		return 1;
	}

}

void PrintHelp() {
	printf("Usage: GrootkitController <Command>\n");
	printf("To see commands use GrootkitController help\n");
}

int Error(const char* message) {
	printf("%s (error=%d)\n", message, GetLastError());
	return 1;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
