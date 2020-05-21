// iLoveThis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define UNICODE 1
#define CHILD_PID_PIPENAME		"\\\\.\\pipe\\get_child_pid"

#include <iostream>
#include <string>
#include <windows.h>

PROCESS_INFORMATION pi;

PROCESS_INFORMATION* StartProcess(std::wstring application_name)
{
	// TODO: Add support for arguments
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start desired process
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
	std::wstring filePath;
	size_t first_index, second_index;
	PROCESS_INFORMATION* hProc;
	HANDLE hPipe;
	std::string result_str;

	filePath = std::wstring(GetCommandLineW());
	first_index = filePath.find_first_of(L'"');
	second_index = filePath.find_first_of(L'"', first_index + 1);

	hProc = StartProcess(filePath.substr(first_index + 1, second_index - first_index - 1));
	if (hProc->dwProcessId == NULL) {
		MessageBoxW(NULL, filePath.substr(first_index + 1, second_index - first_index - 1).c_str(), L"Could not start process", MB_ICONERROR);
		return -1;
	}

	// Loop until pipe server is connected
	while (1)
	{
		hPipe = CreateFileA(
			CHILD_PID_PIPENAME,   // pipe name   	
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

		// Wait 5 seconds to check if a pipe server has been started
		WaitNamedPipeA(CHILD_PID_PIPENAME, 5000);
	}

	// Generate format "{PID}-{TID}"
	result_str = std::to_string(hProc->dwProcessId).append("-").append(std::to_string(hProc->dwThreadId));
	if (!WriteFile(hPipe,
		result_str.c_str(), result_str.length(),
		NULL, NULL // Bytes written, lpOverlapped
	)) {
		MessageBoxA(NULL, "Stub.exe: Error", "Could not write to named pipe", MB_ICONERROR);
	}
	DisconnectNamedPipe(hPipe);

	CloseHandle(hProc);
	CloseHandle(hPipe);
}