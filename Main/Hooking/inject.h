#pragma once
#include <windows.h>
#include <string>

void inject_to_process(DWORD, std::wstring);
std::string get_string_error();