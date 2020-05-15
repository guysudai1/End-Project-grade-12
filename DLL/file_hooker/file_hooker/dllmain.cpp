// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>
#include <string>
#define UNICODE 1

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

            // Sleep(10000);
            // DebugBreak();
            
            if (DetourTransactionBegin() != NO_ERROR) {
                MessageBoxA(NULL, "Could not begin trasncation", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
                MessageBoxA(NULL, "Could not update current thread", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
           
            if (DetourAttach(&(PVOID&)pCreateFileW, myCreateFileW) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach CreateFileA", "Failed", MB_ICONINFORMATION);
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
            // DebugBreak();
            if (DetourTransactionBegin() != NO_ERROR) {
                MessageBoxA(NULL, "Could not begin transcation", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
                MessageBoxA(NULL, "Could not update current thread", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            if (DetourDetach(&(PVOID&)pCreateFileW, myCreateFileW) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach CreateFileA", "Failed", MB_ICONINFORMATION);
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
    DWORD procInfoSize = sizeof(ProcInfoRecv) + content.size() * sizeof(wchar_t);
    auto procInfo = reinterpret_cast<PProcInfoRecv>(new char[sizeof(ProcInfoRecv) + content.size() * sizeof(wchar_t)]);
    procInfo->flags = flags;
    procInfo->pid = GetProcessId(GetCurrentProcess());
    procInfo->procNameSize = content.size(); // in characters
    memcpy(procInfo->procName, content.c_str(), sizeof(wchar_t) * content.size());

    HANDLE hPipe;
    while (true) {
        hPipe = CreateFileA(SEND_PROCESS_INFORMATION, GENERIC_WRITE,
            0, NULL, OPEN_EXISTING,
            0, NULL);

        if (hPipe != INVALID_HANDLE_VALUE) 
            break;

        Sleep(5000);
    }
    std::string procInfoSizeString = std::to_string(procInfoSize);
    char* procInfoSizeCharArray = new char[11]; // max dword string size + 1 for nullbyte
    memset(procInfoSizeCharArray, 0, 11);
    memcpy(procInfoSizeCharArray, procInfoSizeString.c_str(), procInfoSizeString.size() * sizeof(wchar_t));
    if (!WriteFile(hPipe, procInfoSizeCharArray, 11, NULL, NULL)) {
        return;
    }
    if (!WriteFile(hPipe, procInfo, procInfoSize, NULL, NULL)) {
        return;
    }
}

HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    ReportToMainHost(std::wstring(lpFileName), 3);

    // MessageBoxA(NULL, lpFileName, "Open:myCreateFileA", MB_ICONINFORMATION);
    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
