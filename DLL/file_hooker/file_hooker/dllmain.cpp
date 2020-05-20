// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>
#include <string>

#define UNICODE 1

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif


BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    HANDLE hSemaphore;
	HMODULE ntdll, ws2dll;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
			// Acquire symbols at runtime
			ntdll			=					LoadLibraryA("ntdll.dll");
			PPNtCreateFile	= (pNtCreateFile)	GetProcAddress(ntdll, "NtCreateFile");
			PPNtOpenFile	= (pNtOpenFile)		GetProcAddress(ntdll, "NtOpenFile");
			PPNtWriteFile	= (pNtWriteFile)	GetProcAddress(ntdll, "NtWriteFile");
			PPNtReadFile	= (pNtReadFile)		GetProcAddress(ntdll, "NtReadFile");

			// Acquire symbols at runtime
			ws2dll			=					LoadLibraryA("ws2_32.dll");
			PPWSConnect		= (pConnect)		GetProcAddress(ws2dll, "connect");
			PPWSSend		= (pSend)			GetProcAddress(ws2dll, "send");
			PPWSInetNtop	= (pInetNtop)		GetProcAddress(ws2dll, "InetNtopW");
			if (PPWSInetNtop == NULL) {
				PPWSInetNtop = (pInetNtop)		GetProcAddress(ws2dll, "InetNtop");
			}
			pNtohs			= (pntohs)			GetProcAddress(ws2dll, "ntohs");
			PPWSGetPeerName	= (pGetpeername)	GetProcAddress(ws2dll, "getpeername");


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
			
			/* 
				HOOK FUNCTIONS 
			*/

            if (DetourAttach(&(PVOID&)PPNtCreateFile, myNtCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach CreateFileA", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourAttach(&(PVOID&)PPNtOpenFile, myNtOpenFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not attach OpenFileA", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
			if (DetourAttach(&(PVOID&)PPNtWriteFile, myNtWriteFile) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourAttach(&(PVOID&)PPNtReadFile, myNtReadFile) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourAttach(&(PVOID&)PPWSConnect, myConnect) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourAttach(&(PVOID&)PPWSSend, mySend) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
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

            /*
				UNHOOK FUNCTIONS
			*/
            if (DetourDetach(&(PVOID&)PPNtCreateFile, myNtCreateFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach NtCreateFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
            if (DetourDetach(&(PVOID&)PPNtOpenFile, myNtOpenFile) != NO_ERROR) {
                MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
                return FALSE;
            }
			if (DetourDetach(&(PVOID&)PPNtWriteFile, myNtWriteFile) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourDetach(&(PVOID&)PPNtReadFile, myNtReadFile) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourDetach(&(PVOID&)PPWSConnect, myConnect) != NO_ERROR) {
				MessageBoxA(NULL, "Could not detach NtOpenFile", "Failed", MB_ICONINFORMATION);
				return FALSE;
			}
			if (DetourDetach(&(PVOID&)PPWSSend, mySend) != NO_ERROR) {
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
	/*
		Function sends info back to the host process
		sends:
		1. proc_info_size
		2. proc_info
	*/

	wchar_t* full_path;
	DWORD length;
	if (flags == NETWORK_CONNECT || flags == NETWORK_SEND || flags == NETWORK_RECV) {
		full_path = const_cast<wchar_t*>(content.c_str());
		length = content.size();
	}
	else {
		/*
			Get full file access path
		*/
		full_path = new wchar_t[MAX_PATH + 1];
		memset(full_path, 0, (MAX_PATH + 1) * sizeof(wchar_t));
		length = GetFullPathNameW(content.c_str(), MAX_PATH, full_path, NULL);
		if (length == NULL) {
			delete[] full_path;
			return;
		}
	}
	

    DWORD procInfoSize = sizeof(ProcInfoRecv) + (length + 1)* sizeof(wchar_t) ;
    auto procInfo = reinterpret_cast<PProcInfoRecv>(new char[procInfoSize]);
    procInfo->flags = flags;
    procInfo->pid = GetProcessId(GetCurrentProcess());
    procInfo->procNameSize = length; // in characters
    memcpy(procInfo->procName, full_path, sizeof(wchar_t) * length);

	/*
		Pipe opening setup
	*/
    HANDLE hPipe;
	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	OBJECT_ATTRIBUTES ObjectAttributes;
	ObjectAttributes.SecurityDescriptor = 0;
	ObjectAttributes.SecurityQualityOfService = 0;
	ObjectAttributes.RootDirectory = 0;
	ObjectAttributes.Attributes = 0;
	ObjectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);

	wchar_t pipe_path_buf[MAX_PATH + 1];
	memset(pipe_path_buf, 0, (MAX_PATH + 1) * sizeof(wchar_t));
	
	/*
		PIPE PATH
	*/
	UNICODE_STRING pipe_path;
	pipe_path.Buffer = pipe_path_buf;
	lstrcpyW(pipe_path_buf, SEND_PROCESS_INFORMATION);
	pipe_path.Length = lstrlenW(pipe_path_buf) * sizeof(wchar_t);
	pipe_path.MaximumLength = (MAX_PATH + 1)*sizeof(wchar_t);

	ObjectAttributes.ObjectName = &pipe_path;

	/*
		Try to open pipe until succeed
	*/
    while (true) {
		
		NTSTATUS status = PPNtCreateFile(&hPipe,	// PHandle
			FILE_GENERIC_WRITE,						// Access rights
			&ObjectAttributes, &IoStatusBlock, 0,	// Info block (end is allocationsize)
			FILE_ATTRIBUTE_NORMAL,					// FileAttributes
			FILE_SHARE_WRITE,						// ShareAccess
			FILE_OPEN,								// CreateDisposition
			FILE_NON_DIRECTORY_FILE,				// CreateOptions
			NULL, NULL);							// Ea (extended attributes)

		if (NT_SUCCESS(status))
            break;
    }

	// Get process info size
    std::string procInfoSizeString = std::to_string(procInfoSize);
    char* procInfoSizeCharArray = new char[12]; // max dword string size + 1 for nullbyte

    memset(procInfoSizeCharArray, 0, 12);
    memcpy(procInfoSizeCharArray, procInfoSizeString.c_str(), procInfoSizeString.size());
	IO_STATUS_BLOCK NewIoStatusBlock = { 0 };

	// Send process info size to main process
	NTSTATUS writeStatus1 = PPNtWriteFile(hPipe, NULL, NULL, NULL, &NewIoStatusBlock, procInfoSizeCharArray, 12, 0, NULL);
    if (writeStatus1 != 0) {
		// Error
        return;
    }

	// Send actual process info struct
	NTSTATUS writeStatus2 = PPNtWriteFile(hPipe, NULL, NULL, NULL, &NewIoStatusBlock, procInfo, procInfoSize, 0, NULL);
    if (writeStatus2 != 0) {
		// Error
        return;
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}


NTSTATUS (NTAPI myNtCreateFile) (OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PLARGE_INTEGER AllocationSize, IN ULONG FileAttributes, IN ULONG ShareAccess, IN ULONG CreateDisposition, IN ULONG CreateOptions, IN PVOID EaBuffer, IN ULONG EaLength) {
	/*
		hook for NtCreateFile() function
	*/

	// Acquire desired access path from OBJECT_ATTRIBUTES struct
    std::wstring full_string = std::wstring(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
    ReportToMainHost(full_string, REPORT_OPEN);
    return PPNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

NTSTATUS (NTAPI myNtOpenFile) (OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, OUT PIO_STATUS_BLOCK IoStatusBlock, IN ULONG ShareAccess, IN ULONG OpenOptions)
{	
	/*
		hook for NtOpenFile() function
	*/

	// Acquire desired access path from OBJECT_ATTRIBUTES struct
	std::wstring full_string = std::wstring(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
	ReportToMainHost(full_string, REPORT_OPEN);
	return PPNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

NTSTATUS (NTAPI myNtWriteFile) (HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key)
{	
	/*
		hook for NtWriteFile() function
	*/

	wchar_t* pathName = new wchar_t[MAX_PATH + 1];
	memset(pathName, 0, MAX_PATH + 1);
	
	// Acquire full path from handle
	if (GetFinalPathNameByHandleW(FileHandle, pathName, sizeof(wchar_t) * (MAX_PATH + 1), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) == ERROR_PATH_NOT_FOUND) {
		// Couldn't acquire path name from handle
		delete[] pathName;
		return PPNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
	}

	std::wstring full_string = std::wstring(pathName);
	ReportToMainHost(full_string, FILE_WRITE);
	delete[] pathName;

	return PPNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

NTSTATUS(NTAPI myNtReadFile) (HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key)
{
	/*
		hook for NtReadFile() function
	*/

	wchar_t* pathName = new wchar_t[MAX_PATH + 1];
	memset(pathName, 0, MAX_PATH + 1);
	// Acquire full path from handle
	if (GetFinalPathNameByHandleW(FileHandle, pathName, sizeof(wchar_t) * (MAX_PATH + 1), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) == ERROR_PATH_NOT_FOUND) {
		// Couldn't acquire path name from handle
		delete[] pathName;
		return PPNtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
	}
	std::wstring full_string = std::wstring(pathName);
	ReportToMainHost(full_string, FILE_READ);
	delete[] pathName;

	return PPNtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

void send_host_sock_info(const sockaddr* name, DWORD flags) {
	/*
		Send IP information to main process using a sockaddr struct	
	*/

	if (name->sa_family == AF_INET) {
		// IPv4

		const sockaddr_in* info = reinterpret_cast<const sockaddr_in*>(name);
		wchar_t* ip_addr_dst = new wchar_t[16];
		memset(ip_addr_dst, 0, 16 * sizeof(wchar_t));

		if (PPWSInetNtop(info->sin_family, &info->sin_addr, ip_addr_dst, 16) != NULL) {
			// Generate <ip>:<port> wstring
			ReportToMainHost(std::wstring(ip_addr_dst).append(L":").append(std::to_wstring(pNtohs(info->sin_port))), flags);
		}
		delete[] ip_addr_dst;
	}
	else if (name->sa_family == AF_INET6) {
		// IPv6

		const sockaddr_in6 * info = reinterpret_cast<const sockaddr_in6 *>(name);
		wchar_t* ip_addr_dst = new wchar_t[46];
		memset(ip_addr_dst, 0, 46 * sizeof(wchar_t));

		if (PPWSInetNtop(info->sin6_family, &info->sin6_addr, ip_addr_dst, 46) != NULL) {
			// Generate <ip>:<port> wstring
			ReportToMainHost(std::wstring(ip_addr_dst).append(L":").append(std::to_wstring(pNtohs(info->sin6_port))), flags);
		}
		delete[] ip_addr_dst;
	}
}

int (WSAAPI myConnect)( SOCKET s, const sockaddr *name, int namelen) {
	/*
		Hook for socket connect() function
	*/

	send_host_sock_info(name, NETWORK_CONNECT);
	return PPWSConnect(s, name, namelen);
}

int (WSAAPI mySend)( SOCKET s, const char *buf, int len, int flags) {
	/*
		Hook for socket send() function
	*/

	// Try acquiring IPv4 address
	sockaddr_in name;
	memset(&name, 0, sizeof(sockaddr_in));
	int name_length = sizeof(name);
	if (PPWSGetPeerName(s, (SOCKADDR*)&name, &name_length) == 0 && name.sin_family == AF_INET) {
		send_host_sock_info((sockaddr*)(&name), NETWORK_SEND);
		return PPWSSend(s, buf, len, flags);
	}

	// Try acquiring IPv6 address
	sockaddr_in6 ipv6name;
	memset(&ipv6name, 0, sizeof(sockaddr_in6));
	name_length = sizeof(ipv6name);
	if (PPWSGetPeerName(s, (SOCKADDR*)&ipv6name, &name_length) == 0 && ipv6name.sin6_family == AF_INET6) {
		send_host_sock_info((sockaddr*)(&ipv6name), NETWORK_SEND);
	}

	return PPWSSend(s, buf, len, flags);
}