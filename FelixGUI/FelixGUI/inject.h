#pragma once
#include <windows.h>
#include <string>

#ifndef INJECT_H
#define INJECT_H

typedef struct PidStruct {
	DWORD PID; // Process ID
	DWORD TID; // Thread  ID
} PIDStruct, *PPIDStruct;

#define STUB_NAME				L"stub.exe"
#define DLL_NAME				L"file_hooker.dll"
#define CHILD_PID_PIPENAME		"\\\\.\\pipe\\get_child_pid"
#define WAIT_DLL_SEMAPHORE_NAME "resume_thread_dllinjected"

VOID wait_for_injection_and_resume(PPIDStruct);
VOID start_process(std::wstring, std::wstring);
BOOL launch_process(std::wstring, PPIDStruct);
void inject_to_process(DWORD, std::wstring);
std::string get_string_error();
void resume_process(DWORD);

#endif