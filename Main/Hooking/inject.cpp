#include "inject.h"

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

void inject_to_process(DWORD pid, std::wstring dllpath) {
	HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | 
								PROCESS_VM_OPERATION |
								PROCESS_VM_READ		 |
								PROCESS_VM_WRITE,
								FALSE,
								pid
							);
	if (hProc == NULL) {
		MessageBoxA(NULL, "Could not open process", get_string_error().c_str(), MB_ICONERROR);
        return;
	}

    void* hBase;
    hBase = VirtualAllocEx(hProc, 
                           NULL,
                           dllpath.size() * sizeof(wchar_t),
                           0,
                           PAGE_EXECUTE_READWRITE
                           );
    if (hBase == NULL) {
        MessageBoxA(NULL, "Could not allocate enough memory", get_string_error().c_str(), MB_ICONERROR);
        return;
    }

    size_t writtenBytes;

    if (!WriteProcessMemory(hProc,
                            hBase,
                            (LPCVOID)&dllpath[0],
                            dllpath.size() * sizeof(wchar_t),
                            &writtenBytes
                           )) {
        MessageBoxA(NULL, "Could not write to memory", get_string_error().c_str(), MB_ICONERROR);
        return;
    }

    HMODULE loadLibrary = (HMODULE)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryW");
    if (loadLibrary == NULL) {
        MessageBoxA(NULL, "Could not get LoadLibraryW address", get_string_error().c_str(), MB_ICONERROR);
        return;
    }

    HANDLE hThread = CreateRemoteThreadEx(hProc,
                                        NULL,
                                        0,
                                        (LPTHREAD_START_ROUTINE)loadLibrary,
                                        hBase,
                                        0,
                                        NULL,
                                        NULL
                                        );
    if (hThread == NULL) {
        MessageBoxA(NULL, "Could not get create remote thread", get_string_error().c_str(), MB_ICONERROR);
        return;
    }
}