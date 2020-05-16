// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>
#include <string>
#define UNICODE 1

static DWORD my_tid = NULL;
static bool  called_from_my_func = false;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    HANDLE hSemaphore;
    
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
             hSemaphore = OpenSemaphoreA(SYNCHRONIZE | // Security attributes (enables a thread to wait until the object is in the signaled state)
                                         SEMAPHORE_MODIFY_STATE, // required for the ReleaseSemaphore function

                                         FALSE,        // all children don't inherit handler
                                         WAIT_DLL_SEMAPHORE_NAME); // Semaphore name
            if (DetourTransactionBegin() != NO_ERROR) {
                MessageBoxA(NULL, "Could not begin trasncation", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
                MessageBoxA(NULL, "Could not update current thread", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            // pNtCreateFile = GetProcAddress(LoadLibraryA("ntdll.lib"), "NtCreateFile");
            if (DetourAttach(&(PVOID&)PPNtCreateFile, myNtCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach CreateFileA", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourAttach(&(PVOID&)PPNtOpenFile, myNtOpenFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach OpenFileA", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            if (DetourTransactionCommit() != NO_ERROR)
                MessageBoxA(NULL, "Could not add detour function!", "Error", MB_ICONERROR);


            if (hSemaphore != NULL) {
                if (!ReleaseSemaphore(hSemaphore, 1, NULL)) {
                    MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Semaphore release failed", MB_ICONWARNING);
                }
                CloseHandle(hSemaphore);
            }
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            ReportToMainHost(std::wstring(L"Close"), 1);
            if (DetourTransactionBegin() != NO_ERROR) {
                MessageBoxA(NULL, "Could not begin transcation", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
                MessageBoxA(NULL, "Could not update current thread", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            // pNtCreateFile = GetProcAddress(LoadLibraryA("ntdll.lib"), "NtCreateFile");
            if (DetourDetach(&(PVOID&)PPNtCreateFile, myNtCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach NtCreateFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourDetach(&(PVOID&)PPNtOpenFile, myNtOpenFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            if (DetourTransactionCommit() != NO_ERROR)
                MessageBoxA(NULL, "Could not remove detour function!", "Error", MB_ICONERROR);   

            break;
    }
    return TRUE;
}


void ReportToMainHost(std::wstring content, DWORD flags)
{
    wchar_t* full_path = new wchar_t[MAX_PATH + 1];
    memset(full_path, 0, (MAX_PATH + 1) * sizeof(wchar_t));
    DWORD length = GetFullPathNameW(content.c_str(), MAX_PATH, full_path, NULL);
    if (length == NULL) {
        delete[] full_path;
        return;
    }
    DWORD procInfoSize = sizeof(ProcInfoRecv) + (length + 1)* sizeof(wchar_t) ;
    auto procInfo = reinterpret_cast<PProcInfoRecv>(new char[procInfoSize]);
    procInfo->flags = flags;
    procInfo->pid = GetProcessId(GetCurrentProcess());
    procInfo->procNameSize = length; // in characters
    memcpy(procInfo->procName, full_path, sizeof(wchar_t) * length);

    HANDLE hPipe;
    // Leaving while loop incase I want to add sleeps and checks
    while (true) {
        my_tid = GetCurrentThreadId();
        called_from_my_func = true;
        hPipe = CreateFileA(SEND_PROCESS_INFORMATION, GENERIC_WRITE,
            0, NULL, OPEN_EXISTING,
            0, NULL);
        my_tid = NULL;
        if (hPipe != INVALID_HANDLE_VALUE) 
            break;
        
        return;
    }
    std::string procInfoSizeString = std::to_string(procInfoSize);
    char* procInfoSizeCharArray = new char[12]; // max dword string size + 1 for nullbyte
    memset(procInfoSizeCharArray, 0, 12);
    memcpy(procInfoSizeCharArray, procInfoSizeString.c_str(), procInfoSizeString.size());
    if (!WriteFile(hPipe, procInfoSizeCharArray, 12, NULL, NULL)) {
        return;
    }
    if (!WriteFile(hPipe, procInfo, procInfoSize, NULL, NULL)) {
        return;
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

/*
HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    ReportToMainHost(std::wstring(lpFileName), 3);
    // MessageBoxA(NULL, lpFileName, "Open:myCreateFileA", MB_ICONINFORMATION);
    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


HANDLE WINAPI myCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    std::string filename(lpFileName);
    ReportToMainHost(std::wstring(filename.begin(), filename.end()), 3);
    // MessageBoxA(NULL, lpFileName, "Open:myCreateFileA", MB_ICONINFORMATION);
    return pCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
*/
NTSTATUS (NTAPI myNtCreateFile) (
    OUT PHANDLE           FileHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN PLARGE_INTEGER     AllocationSize,
    IN ULONG              FileAttributes,
    IN ULONG              ShareAccess,
    IN ULONG              CreateDisposition,
    IN ULONG              CreateOptions,
    IN PVOID              EaBuffer,
    IN ULONG              EaLength
    ) {
    DWORD current_proc_id = GetCurrentProcessId();
    if (!called_from_my_func) {
        called_from_my_func = false;
        wchar_t* pathName = ObjectAttributes->ObjectName->Buffer;
        /*
        if (GetFinalPathNameByHandleW(ObjectAttributes->ObjectName, pathName, sizeof(wchar_t) * (MAX_PATH + 1), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) == ERROR_PATH_NOT_FOUND)
            return PPNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
            */
        std::wstring full_string = std::wstring(pathName);
        full_string.append(L"(");
        if ((DesiredAccess & FILE_GENERIC_READ) != 0)
            full_string.append(L"R");
        if ((DesiredAccess & FILE_GENERIC_WRITE) != 0)
            full_string.append(L"W");
        if ((DesiredAccess & FILE_GENERIC_EXECUTE) != 0)
            full_string.append(L"X");
        full_string.append(L")");
        ReportToMainHost(full_string, 3);
    }
    return PPNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

NTSTATUS (NTAPI myNtOpenFile) (OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, OUT PIO_STATUS_BLOCK IoStatusBlock, IN ULONG ShareAccess, IN ULONG OpenOptions)
{
    MessageBoxW(NULL, L"In ntopenfile", L"I did it", MB_ICONINFORMATION);
    return PPNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}
