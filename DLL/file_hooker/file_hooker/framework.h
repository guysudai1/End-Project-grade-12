#pragma once


#ifndef FRAMEWORK_H
#define FRAMEWORK_H
// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
#include <string>
#include <winternl.h>
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

void ReportToMainHost(std::wstring, DWORD);

/*
HANDLE (WINAPI *pCreateFileW) (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;
HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

HANDLE(WINAPI* pCreateFileA) (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;
HANDLE WINAPI myCreateFileA (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
*/


typedef NTSTATUS (NTAPI *pNtCreateFile)(
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
);

NTSTATUS (NTAPI myNtCreateFile)(
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
    );


typedef NTSTATUS(NTAPI* pNtOpenFile)(
    OUT PHANDLE           FileHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG              ShareAccess,
    IN ULONG              OpenOptions
    );

NTSTATUS (NTAPI myNtOpenFile)(
    OUT PHANDLE           FileHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG              ShareAccess,
    IN ULONG              OpenOptions
);

static pNtCreateFile PPNtCreateFile = (pNtCreateFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateFile");
static pNtOpenFile PPNtOpenFile = (pNtOpenFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtOpenFile");

#endif