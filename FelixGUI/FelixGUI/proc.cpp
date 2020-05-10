#include "proc.h"
#include <psapi.h>
#include <string>


#define MAX_PROCS_INIT 1024

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
			procDependencies.append(L"; ");
		}
		// Release heap memory
		delete[] tempProcName;
	
	}
	delete[] procModules;
	return procDependencies;
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
