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
           
            if (DetourAttach(&(PVOID&)pCreateFileA, myCreateFileA) != NO_ERROR) {
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

            if (DetourDetach(&(PVOID&)pCreateFileA, myCreateFileA) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach CreateFileA", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }

            if (DetourTransactionCommit() != NO_ERROR)
                MessageBoxA(NULL, "Could not remove detour function!", "Error", MB_ICONERROR);   
            break;
    }
    return TRUE;
}

HANDLE WINAPI myCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    MessageBoxA(NULL, lpFileName, "Open:myCreateFileA", MB_ICONINFORMATION);
    return pCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);;
}
