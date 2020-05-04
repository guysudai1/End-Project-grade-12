// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>
// #include <string>
#define UNICODE 1

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    int a = 0;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:

            // Sleep(10000);
            // MessageBoxW(NULL, L"I\'m in the process", L"Success", 0);

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
            /*
            pCreateFileW = CreateFileW;
            if (DetourAttach(&(PVOID&)pCreateFileW, myCreateFileW) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach CreateFileW", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            pCreateFile = CreateFile;
            if (DetourAttach(&(PVOID&)pCreateFile, myCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach CreateFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            */
            if (DetourTransactionCommit() != NO_ERROR)
                MessageBoxA(NULL, "Could not add detour function!", "Error", MB_ICONERROR);
            // MessageBox(NULL, L"Hello from this process", L"I was injected!", MB_ICONINFORMATION);
            
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
            /*
            pCreateFileW = CreateFileW;
            if (DetourDetach(&(PVOID&)pCreateFileW, myCreateFileW) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach CreateFileW", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            pCreateFile = CreateFile;
            if (DetourDetach(&(PVOID&)pCreateFile, myCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach CreateFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }*/
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

/*
HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    MessageBoxW(NULL, std::wstring(lpFileName).c_str(), L"Open:myCreateFileW", MB_ICONINFORMATION);
    return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


HANDLE myCreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    MessageBoxW(NULL, std::wstring(lpFileName).c_str(), L"Open:myCreateFile", MB_ICONINFORMATION);
    return CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
*/