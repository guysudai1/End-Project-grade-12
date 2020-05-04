// iLoveThis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define UNICODE 1

#include <iostream>
#include <string>
#include <windows.h>


PROCESS_INFORMATION pi;

PROCESS_INFORMATION* StartProcess(std::wstring application_name)
{
	// additional information
	STARTUPINFO si;
	

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	if (!CreateProcess(application_name.c_str(),   // Executable path
		NULL,				// Command line
		NULL,				// Process handle not inheritable
		NULL,				// Thread handle not inheritable
		FALSE,				// Set handle inheritance to FALSE
		CREATE_SUSPENDED,   // Create process as suspended
		NULL,				// Use parent's environment block
		NULL,				// Use parent's starting directory 
		&si,				// Pointer to STARTUPINFO structure
		&pi					// Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	)) {
		MessageBoxW(NULL, L"Stub.exe: Error starting process", (std::wstring(L"Could not start process ") + application_name).c_str(), MB_ICONERROR);
		return NULL;
	}
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return &pi;

}

int main(int argc, char* argv[])
{
	std::wstring filePath(GetCommandLineW());
	size_t first_index = filePath.find_first_of(L'"');
	size_t second_index = filePath.find_first_of(L'"', first_index + 1);
	// MessageBox(NULL, filePath.substr(first_index + 1, second_index - first_index - 1).c_str(), L"Program to execute", 1);
	PROCESS_INFORMATION* hProc = StartProcess(filePath.substr(first_index + 1, second_index - first_index - 1));
	if (hProc->dwProcessId == NULL) {
		MessageBoxW(NULL, filePath.substr(first_index + 1, second_index - first_index - 1).c_str(), L"Could not start process", MB_ICONERROR);
	}

	HANDLE hPipe;
	std::string pipename("\\\\.\\pipe\\get_child_pid");
	while (1)
	{
		hPipe = CreateFileA(
			pipename.c_str(),   // pipe name   	
			GENERIC_WRITE,	// read and write access 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

	  // Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			MessageBox(NULL, L"Stub.exe: Error could not open pipe", L"Pipe is busy", MB_ICONERROR);
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipeA(pipename.c_str(), 5000))
		{
			printf("Could not open pipe: 5 second wait timed out.");
		}
	}
	DWORD number_of_read;

	if (!WriteFile(
		hPipe,
		std::to_string(hProc->dwProcessId).append("-").append(std::to_string(hProc->dwThreadId)).c_str(),
		std::to_string(hProc->dwProcessId).append("-").append(std::to_string(hProc->dwThreadId)).length(),
		&number_of_read,
		NULL
	)) {
		MessageBoxA(NULL, "Stub.exe: Error", "Could not write to named pipe", MB_ICONERROR);
	}
	DisconnectNamedPipe(hPipe);
}


