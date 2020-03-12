#include "hooking.h"
#include "hashmap.h"
#include "sha256.h"
#include <io.h>
#include <fcntl.h>
#define UNICODE 1
#include <windows.h>

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
	wchar_t* key = (wchar_t*)L"ddddddsa";
	std::wstring value(L"dsadsa");
	char buf[1000] = { 0 };
	
	int ret = std::wcstombs(buf, key, sizeof(buf));
	if (ret == 1000) buf[999] = '\0';
	std::string hash_this(buf, 0, strlen(buf));

	hash_map* map = new hash_map(10000);
	
	std::wcout << key << std::endl;
	std::wcout << map->get_value(key) << std::endl;

	std::string _ = sha256((unsigned char*) buf, strlen(buf));
	std::wstring encoded(_.begin(), _.end());
	
	hook_folder(L"C:/Users/USER11/Desktop/End-Project-grade-12-master/weird_folder/", map, true);
	return 0; 
}	
