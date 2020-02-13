// Hooking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "hooking.h"
#include <algorithm>


/*
	PARAMETERS: 
		FileName = name of file to open
	RETURN VALUE:
		returns HANDLE of opened file

*/

HANDLE open_monitored_file(std::string PathName) {
	HANDLE file;
	// std::replace(FileName.begin(), FileName.end(), '\\', '/');
	std::cout << PathName << std::endl;
	if (PathName.back() == '\\') {
		std::cout << "Opened directory: " << PathName << std::endl;
		file = CreateFileA(
			PathName.c_str(),					// File name
			FILE_LIST_DIRECTORY,				// Open as directory
			LET_ALL_PROCS_OPEN,					// Allow other processes to interact with this folder
			NULL,								// Cannot be inherited by children of program
			OPEN_EXISTING,						// Opens existing file flag
			FILE_FLAG_BACKUP_SEMANTICS,			// Ignored when opening file
			NULL								// Ignored when opening file
		);
	}
	else {
		std::cout << "Opened file: " << PathName << std::endl;
		file = CreateFileA(
			PathName.c_str(),					// File name
			GENERIC_READ,						// Open as directory
			LET_ALL_PROCS_OPEN,					// Allow other processes to interact with this folder
			NULL,								// Cannot be inherited by children of program
			OPEN_EXISTING,						// Opens existing file flag
			FILE_FLAG_BACKUP_SEMANTICS,			// Ignored when opening file
			NULL								// Ignored when opening file
		);
	}
	std::cout << "Error: " << std::to_string(GetLastError()) << std::endl;
	return file;
}

BOOL monitor_folder(HANDLE file, bool watch_subtree) {
	return false;
	//return ReadDirectoryChangesExW(
	//	file
	//	);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
