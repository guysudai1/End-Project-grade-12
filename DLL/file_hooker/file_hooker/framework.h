#pragma once

#ifndef FRAMEWORK_H
#define FRAMEWORK_H
// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <string>
#include <winternl.h>
// flags: 
// 0x00000001 CLOSED PROCESS
// 0x00000002 NETWORK
// 0x00000003 FILE (OPEN)
// 0x00000004 FILE (READ)
// 0x00000005 FILE (WRITE)

#define CLOSED_PROCESS  0x1
#define REPORT_OPEN	    0x2
#define FILE_READ	    0x3
#define FILE_WRITE	    0x4
#define NETWORK_CONNECT 0x5
#define NETWORK_SEND	0x6
#define NETWORK_RECV	0x7

typedef struct _procInfoRecv {
	DWORD pid;
	DWORD flags;
	size_t procNameSize; // in characters
	wchar_t procName[];
} ProcInfoRecv, *PProcInfoRecv;

#define SEND_PROCESS_INFORMATION L"\\??\\pipe\\procInfoPipe"
#define WAIT_DLL_SEMAPHORE_NAME  "resume_thread_dllinjected"

void ReportToMainHost(std::wstring, DWORD);


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

typedef NTSTATUS(NTAPI* pNtOpenFile)(
	OUT PHANDLE           FileHandle,
	IN ACCESS_MASK        DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK  IoStatusBlock,
	IN ULONG              ShareAccess,
	IN ULONG              OpenOptions
	);

typedef NTSTATUS(NTAPI* pNtWriteFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

typedef NTSTATUS(NTAPI* pNtReadFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

/*
	OPEN FILE OPERATIONS FUNCTIONS
*/

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

NTSTATUS (NTAPI myNtOpenFile)(
    OUT PHANDLE           FileHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG              ShareAccess,
    IN ULONG              OpenOptions
);

/*
	WRITE FILE FUNCTIONS
*/

NTSTATUS (NTAPI myNtWriteFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
);

/*
	READ FILE FUNCTIONS
*/

NTSTATUS(NTAPI myNtReadFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);


/*
	NETWORK FUNCTIONS
*/

typedef int (WSAAPI* pConnect)(
	SOCKET         s,
	const sockaddr *name,
	int            namelen
);

typedef int (WSAAPI* pSend)(
	SOCKET     s,
	const char *buf,
	int        len,
	int        flags
);

typedef PCWSTR (WSAAPI* pInetNtop)(
	INT        Family,
	const VOID *pAddr,
	PWSTR      pStringBuf,
	size_t     StringBufSize
);

typedef u_short (WSAAPI* pntohs)(
	u_short netshort
);

typedef int (WSAAPI* pGetpeername)(
	SOCKET   s,
	sockaddr *name,
	int      *namelen
);

/*
	MY NETWORK FUNCTIONS
*/
int (WSAAPI myConnect)(
	SOCKET         s,
	const sockaddr *name,
	int            namelen
);

int (WSAAPI mySend)(
	SOCKET     s,
	const char *buf,
	int        len,
	int        flags
	);

static pConnect			PPWSConnect;
static pSend			PPWSSend;
static pInetNtop		PPWSInetNtop;
static pntohs			pNtohs;
static pGetpeername		PPWSGetPeerName;

static pNtCreateFile	PPNtCreateFile; // = (pNtCreateFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateFile");
static pNtOpenFile		PPNtOpenFile; // = (pNtOpenFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtOpenFile");
static pNtWriteFile		PPNtWriteFile; // = (pNtWriteFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtWriteFile");
static pNtReadFile		PPNtReadFile; // = (pNtReadFile)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtReadFile");
#endif