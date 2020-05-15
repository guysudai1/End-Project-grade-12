#pragma once


#ifndef FRAMEWORK_H
#define FRAMEWORK_H
// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
#include <string>
// flags: 
// 0x00000001 CLOSED PROCESS
// 0x00000002 NETWORK
// 0x00000003 FILE (OPEN)
// 0x00000004 FILE (READ)
// 0x00000005 FILE (WRITE)

typedef struct _procInfoRecv {
	DWORD pid;
	DWORD flags;
	size_t procNameSize; // in characters
	wchar_t procName[];
} ProcInfoRecv, *PProcInfoRecv;

#define SEND_PROCESS_INFORMATION "\\\\.\\pipe\\procInfoPipe"
#define WAIT_DLL_SEMAPHORE_NAME  "resume_thread_dllinjected"

void ReportToMainHost(std::string, std::wstring, DWORD);

HANDLE (WINAPI *pCreateFileW) (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;
HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

#endif