#include "hooking.h"
#include "hashmap.h"
#include <io.h>
#include <fcntl.h>
#define UNICODE 1
#include <windows.h>
#include "main.h"

int main()
{
	/*
	_setmode(_fileno(stdout), _O_U8TEXT);
	std::wstring file_monitor;
	std::wcout << L"Starting..." << std::endl;
	std::getline(std::wcin, file_monitor);

	wchar_t buf[1024];
	GetCurrentDirectoryW(1024, buf);

	std::wstring file_path = std::wstring(&buf[0], &buf[wcslen(buf)]);
	file_path.append(L"\\");
	OVERLAPPED overlapped = {0};
	overlapped.hEvent = CreateEvent(0, 0, 0, 0);
	HANDLE file = open_monitored_file(file_path);
	memset(buf, 0, wcslen(buf));

	MONITER_FOLDER(buf, file, true, overlapped);
	// FILE_NOTIFY_INFORMATION* infoBuf = (FILE_NOTIFY_INFORMATION*)(&buf[0]);

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::wcout << L"Filename: " << std::endl;
	//std::wcout << infoBuf->FileName << std::endl;	
	FILE_NOTIFY_INFORMATION *info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buf);
	do {
		std::wstring str(info->FileName, info->FileNameLength / sizeof(wchar_t));
		std::wcout << str << std::endl;
		MessageBoxW(NULL, info->FileName, info->FileName, 1);
		info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>((BYTE*)info + info->NextEntryOffset);
	} while (info->NextEntryOffset > 0);

	CloseHandle(file);
	*/
	std::wcout << L"hey" << std::endl;
	char hey[4] = "hey";
	char wtf[4] = "wtf";
	hash_map<char*, char*>* map = new hash_map<char*, char*>(10000);
	map->put(hey, wtf);
	std::wcout << "hello there" << std::endl;
	return 0; 
}	
