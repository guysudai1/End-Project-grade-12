#include "hooking.h"
#include "hashmap.h"

int main()
{
	std::string file_monitor;
	std::cout << "Starting..." << std::endl;
	std::getline(std::cin, file_monitor);

	char buf[1024];
	GetCurrentDirectoryA(1024, buf);

	std::string file_path = std::string(buf);
	file_path.append("\\");
	OVERLAPPED overlapped = {0};
	overlapped.hEvent = CreateEvent(0, 0, 0, 0);
	HANDLE file = open_monitored_file(file_path);
	memset(buf, 0, 1024);

	MONITER_FOLDER(buf, file, true, overlapped);
	FILE_NOTIFY_INFORMATION* infoBuf = (FILE_NOTIFY_INFORMATION*)(buf);
	std::wstring str(infoBuf->FileName, infoBuf->FileNameLength / sizeof(wchar_t));

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::cout << infoBuf->FileName << std::endl;
	CloseHandle(file);
	return 0; 
}	
