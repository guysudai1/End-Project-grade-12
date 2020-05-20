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
std::string get_string_error();

class Injector {
private:
	std::wstring application_name;
	PPIDStruct child;
public:
	Injector(std::wstring);
	~Injector();
	VOID wait_for_injection_and_resume();
	VOID start_process();
	BOOL launch_process();
	DWORD get_pid();
	void inject_to_process(DWORD, std::wstring);
	void resume_process();
};
#endif