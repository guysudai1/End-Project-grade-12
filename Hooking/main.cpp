#include "hooking.h"
#include "hashmap.h"

int main()
{
	std::string file_monitor;
	std::cout << "Starting..." << std::endl;
	std::getline(std::cin, file_monitor);

	char* buf = (char*) malloc(1024 * sizeof(char));
	GetCurrentDirectoryA(1024, buf);

	std::string file_path = std::string(buf);
	free(buf);
	file_path.append("\\");

	HANDLE file = open_monitored_file(file_path);
	CloseHandle(file);
	return 0; 
}	
