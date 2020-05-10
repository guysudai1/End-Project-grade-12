#pragma once
#include <Windows.h>
#include <vector>

#ifndef PROC_H
#define PROC_H

typedef struct MyEnumWindowsStruct {
	DWORD pid;
	std::vector<HWND>* hwnd;
} PIDWindows, * PPIDWindows;

/* Helper functions for drawing processes */
DWORD* get_procs(DWORD*);
HMODULE* get_proc_modules(HANDLE, DWORD*);
void get_hwnd_from_pid(DWORD, std::vector <HWND>&);
BOOL CALLBACK EnumWins(HWND, LPARAM);
#endif