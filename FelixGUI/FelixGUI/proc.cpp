#include "proc.h"
#include "icon_functions.h"
#include <psapi.h>
#include <string>

#define EXE_ICON_PATH "Resources/Icons/exe.ico"

#define MAX_PROCS_INIT 1024

// Small icon
#define GCL_HICONSM -34

QIcon* acquire_proc_icon(DWORD pid, wchar_t* procName) {
	QIcon* qtpIcon = nullptr;
	std::vector<HWND> windows;
	HICON pIcon;
	PIDWindows windStruct;

	windStruct.hwnd = &windows;
	windStruct.pid = pid;

	/* Enumerate over all windows to acquire windows with PID of `pid` */
	EnumWindows(EnumWins, (LPARAM)&windStruct);

	/* Attempt to get icon of windows*/
	for (int wind = 0; wind < windows.size(); wind++) {
		pIcon = (HICON)GetClassLongA(windows[wind], GCL_HICONSM);
		int error = GetLastError();
		error = error;
		if (pIcon != 0) {
			QPixmap map = fromHICON(pIcon);
			qtpIcon = new QIcon(map);
			break;
		}

	}
	/* Attempt to get icon of executable (from path extracted) */
	if (qtpIcon == nullptr) {
		if (ExtractIconExW(procName, 0, NULL, &pIcon, 1) == 1) {
			qtpIcon = new QIcon(fromHICON(pIcon));
		}
	}
	/* Set icon to default executable icon if both earlier failed*/
	if (qtpIcon == nullptr) {
		qtpIcon = new QIcon(EXE_ICON_PATH);
	}

	return qtpIcon;
}

HANDLE attempt_open_process(DWORD pid) {
	HANDLE hProc;

	hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
		FALSE,
		pid);

	if (hProc == NULL) {
		// Attempt to open process with LIMITED information.
		DWORD error = GetLastError();
		hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION,
			FALSE,
			pid);
		if (hProc == NULL) {
			DWORD error = GetLastError();
			return NULL;
		}
	}

	return hProc;
}

HMODULE* get_proc_modules(HANDLE hProc, DWORD* size) {
	HMODULE* procs;
	DWORD bytesReturned;
	DWORD current_mem = MAX_PROCS_INIT;
	do {
		procs = new HMODULE[current_mem];
		if (EnumProcessModules(hProc, procs, current_mem * sizeof(DWORD), &bytesReturned) == 0) {
			MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Error: get_procs", MB_ICONERROR);
		}
		if (bytesReturned / sizeof(DWORD) == current_mem) {
			current_mem *= 2;
			delete[] procs;
			procs = nullptr;
		}
		else { break; }
	} while (true);
	*size = bytesReturned / sizeof(DWORD);
	return procs;
}


DWORD* get_procs(DWORD* size) {
	DWORD* procs;
	DWORD bytesReturned;
	DWORD current_mem = MAX_PROCS_INIT;
	do {
		procs = new DWORD[current_mem];
		if (EnumProcesses(procs, current_mem * sizeof(DWORD), &bytesReturned) == 0) {
			MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Error: get_procs", MB_ICONERROR);
		}
		if (bytesReturned / sizeof(DWORD) == current_mem) {
			current_mem *= 2;
			delete[] procs;
			procs = nullptr;
		}
		else { break; }
	} while (true);
	*size = bytesReturned / sizeof(DWORD);
	return procs;
}

BOOL CALLBACK EnumWins(HWND hWnd,
	LPARAM lParam) {
	DWORD PID = 0;
	GetWindowThreadProcessId(hWnd, &PID);
	if (PID == ((PPIDWindows)lParam)->pid) {
		((PPIDWindows)lParam)->hwnd->push_back(hWnd);
	}
	return TRUE;
}

BOOL get_proc_bitness(DWORD pid) {
	USHORT processMachine, arcitectureNative;
	HANDLE hProc = attempt_open_process(pid);
	if (IsWow64Process2(hProc, &processMachine, &arcitectureNative)) {
		return processMachine == IMAGE_FILE_MACHINE_UNKNOWN;
	}

	CloseHandle(hProc);
	return FALSE;
}

std::wstring get_all_dependencies(HANDLE hProc) {

	/* Acquire process dependencies */
	DWORD moduleSize;
	HMODULE* procModules = get_proc_modules(hProc, &moduleSize);
	std::wstring procDependencies;

	/* Process dependencies to wstring (concat with "; ") */

	for (int j = 0; j < moduleSize; j++) {
		wchar_t* tempProcName = new wchar_t[MAX_PATH];
		if (GetModuleBaseNameW(hProc, procModules[j], tempProcName, MAX_PATH) != 0) {
			procDependencies.append(tempProcName);
			procDependencies.append(L"\n");
		}
		// Release heap memory
		delete[] tempProcName;
	
	}
	delete[] procModules;
	return procDependencies.substr(0, procDependencies.size() - 1);
}

void get_hwnd_from_pid(DWORD pid, std::vector <HWND>& vhWnds) {
	HWND hCurWnd = nullptr;
	do
	{
		// Enumerate over all the windows
		hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
		DWORD checkProcessID = 0;
		GetWindowThreadProcessId(hCurWnd, &checkProcessID);
		if (checkProcessID == pid)
		{
			vhWnds.push_back(hCurWnd);
		}
	} while (hCurWnd != nullptr);
}
