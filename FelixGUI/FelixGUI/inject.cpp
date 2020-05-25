#include "inject.h"

// Stolen this incredibly comfortable function from
// https://stackoverflow.com/a/17387176
std::string get_string_error()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

VOID Injector::start_process()
{
    /*
    Actual call to stub.exe (intermediate) in order to launch desired process detached
    from parent process of stub.exe
    */

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Zero structs
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	std::wstring app_str =  (std::wstring(STUB_NAME) + L" \"" + application_name + L"\"");
    // Start stub program
	INT err = CreateProcess(NULL,   // Path to stub
		(LPWSTR)app_str.c_str(),        // Command line (desired program to launch)
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
    // Close process and thread handles. 
	if (err != 0) {
		err = GetLastError();
	}
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

Injector::Injector(std::wstring path)
{
	this->application_name = path;
}

Injector::~Injector()
{
}

DWORD Injector::get_pid() {
	return this->child->PID;
}

VOID Injector::wait_for_injection_and_resume()
{
    /*
    Detecting end of dll injection, whenever it finishes the FileMon resumes the main thread and lets the 
    process continue as natural. (after being hooked)
    */
    HANDLE hSemaphore;

    // Create semaphore in order to detect end of dll injection
    hSemaphore = CreateSemaphoreA(NULL,  // Security attributes
        0,		                         // lInitialCount = 1
        5,		                         // maxCount
        WAIT_DLL_SEMAPHORE_NAME		     // Semaphore name
    );

    if (hSemaphore == NULL) {
        MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Semaphore creation failed", MB_ICONWARNING);
        return;
    }
    
	wchar_t full_dll_path[MAX_PATH + 1];
	if (GetCurrentDirectoryW(MAX_PATH + 1, full_dll_path) != 0) {
		this->inject_to_process(this->child->PID, std::wstring(full_dll_path).append(L"\\").append(DLL_NAME) );
		// Wait for DLL injection to end
		WaitForSingleObject(hSemaphore, INFINITE);
		this->resume_process();
	}
    CloseHandle(hSemaphore);
}

BOOL Injector::launch_process()
{
    /*
    In order to call an executable and prevent it from being the child of the current process
    we call a stub.exe file with the only purpose of launching an executable, and shutting it down.
    (All of this is done in order to avoid anti-debugging measures)

    So we have a situation like:
    -> Current Process 
        ->  Stub.exe
            -> Desired process (SUSPENDED)

    turns into:
    -> Current Process
    -> Desired Process (SUSPENDED)

    NOTE:
    Creating desired process as suspended in order to have time to inject it
    */

	HANDLE hPipe; // Store named pipe handle           
	bool connected = false; // Check if one pipe has connected
	char buf[1024]; // Buffer to store {PID}-{TID} string 
	std::string str_buf; // Stores buffer in string form 
	DWORD read_amount; // Used temp while reading pipe
	size_t seperator; // Used temp for finding index of separator

	// Create named pipe for transferring PID and TID of desired process to inject
	hPipe = CreateNamedPipeA(CHILD_PID_PIPENAME,
		PIPE_ACCESS_INBOUND,
		PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "CreateNamedPipe failed", std::to_string(GetLastError()).c_str(), MB_ICONWARNING);
		return FALSE;
	}

	// Call Stub.exe and create process separated from current process (not child of)
	// TODO: Remove constant path
	this->start_process();

	while (!connected && hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, buf, sizeof(buf) - 1, &read_amount, NULL) != FALSE)
			{
				// Terminate string
				buf[read_amount] = '\0';
				// 1 Pipe max read.
				connected = true;
			}

			DisconnectNamedPipe(hPipe);
		}

	}

	CloseHandle(hPipe);

	// Extract PID and TID from pipe
	str_buf = std::string(buf);
	seperator = str_buf.find('-');
	PIDStruct* child_struct = new PIDStruct;
	child_struct->PID = std::stoi(str_buf.substr(0, seperator));
	child_struct->TID = std::stoi(str_buf.substr(seperator + 1, str_buf.length() - seperator));
	this->child = child_struct;

    return TRUE;
}

void Injector::inject_to_process(DWORD pid, std::wstring dllpath) 
{
    /*
    Injects DLL to process with PID of pid. 
    Uses reflected DLL injection: (Graph below)
    Current Process -> []
    */
    void* hBase;
    HANDLE hProc;

    hProc = OpenProcess(PROCESS_CREATE_THREAD | // Required to use CreateRemoteThread
								PROCESS_VM_OPERATION | // Required to use VirtualProtectEx, WriteProcessMemory
								PROCESS_VM_READ		 | // Required to read from memory
								PROCESS_VM_WRITE,      // Required to write into memory
								FALSE,                 // Children of this process will not inherit this handle           
								pid                    // PID to open
							);
	if (hProc == NULL) {
		MessageBoxA(NULL,  get_string_error().c_str(), "Could not open process", MB_ICONERROR);
        return;
	}

	// MessageBoxW(NULL, dllpath.c_str(), L"Injecting to", MB_ICONINFORMATION);
    // Allocate executable|writable memory area in desired process
    hBase = VirtualAllocEx(hProc,           // proc handler
                           NULL,            // Let the function decide where the memory will be
                           dllpath.size() * sizeof(wchar_t), // Size desired to inject (remember it's wstring)
                           MEM_RESERVE |    // MEM_RESERVE - consider memory area in use, 
                           MEM_COMMIT,      // MEM_COMMIT - actually commit changes to memory area
                           PAGE_EXECUTE_READWRITE // Get page with RWX
                           );
    if (hBase == NULL) {
        MessageBoxA(NULL, get_string_error().c_str(), "Could not allocate enough memory", MB_ICONERROR);
        return;
    }

    if (!WriteProcessMemory(hProc,          // proc handler
                            hBase,          // use address allocated earlier
                            (LPCVOID)&dllpath[0], // String to copy
                            dllpath.size() * sizeof(wchar_t), // Size of string
                            NULL            // Don't want to get amount of bytes written
                           )) {
        MessageBoxA(NULL, get_string_error().c_str(), "Could not write to memory", MB_ICONERROR);
        return;
    }

    // Find LoadLibraryW function in kernel32.dll of the current process (assumed kernel32.dll is loaded in the same place on the other process)
    HMODULE loadLibrary = (HMODULE)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryW");
    if (loadLibrary == NULL) {
        MessageBoxA(NULL, get_string_error().c_str(), "Could not get LoadLibraryW address", MB_ICONERROR);
        return;
    }

    // Run LoadLibraryW(DLL_location) on the other process
    HANDLE hThread = CreateRemoteThread(hProc,            // proc handler
                                        NULL, 0,          // lpThreadAttributes, dwStackSize
                                        (LPTHREAD_START_ROUTINE)loadLibrary, // function to run
                                        hBase,            // Parameter to function
                                        0,                // Run immediately after creation
                                        NULL              // lpThreadId
                                        );
    if (hThread == NULL) {
        MessageBoxA(NULL, get_string_error().c_str(), "Could not get create remote thread", MB_ICONERROR);
        return;
    }

    CloseHandle(hThread);
    CloseHandle(hProc);
}

void Injector::resume_process() {
    /*
    (NOTE) Created process as SUSPENDED in order to have time to inject to it [without missing hooks], 
    so in order for it to run we need to awaken its main thread.
    */

    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, this->child->TID);
    ResumeThread(hThread);
    CloseHandle(hThread);
}