#pragma once
#include <Windows.h>
#include <vector>
#include <QIcon>

#ifndef PROC_H
#define PROC_H

typedef struct MyEnumWindowsStruct {
	DWORD pid;
	std::vector<HWND>* hwnd;
} PIDWindows, * PPIDWindows;

/* Helper functions for drawing processes */
DWORD* get_procs(DWORD*);
HMODULE* get_proc_modules(HANDLE, DWORD*);
BOOL get_proc_bitness(DWORD);
std::wstring get_all_dependencies(HANDLE);
HANDLE attempt_open_process(DWORD);
QIcon* acquire_proc_icon(DWORD, wchar_t*);
void get_hwnd_from_pid(DWORD, std::vector <HWND>&);
BOOL CALLBACK enum_wins(HWND, LPARAM);
#endif