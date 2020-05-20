#include <iostream>
#include <ws2tcpip.h>
#include <Windows.h>
#include <string>

#define PORT 80

int main()
{
    std::cout << "Hello World!\n";
    system("pause");
    HANDLE file = CreateFileW(L"ExampleFileTest.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	char* buf = new char[100];
	WriteFile(file, "PleaseWrite", 11, NULL, NULL);
	ReadFile(file, buf, 99, NULL, NULL);
    CloseHandle(file);
    /*for (int i = 0; i < 10; ++i) {
        HANDLE newFile = CreateFileW(L"ExampleFileTest.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(newFile, "PleaseWrite", 11, NULL, NULL);
        CloseHandle(newFile);
    }*/
	std::cout << sizeof(sockaddr) << std::endl;
	std::cout << sizeof(sockaddr_in) << std::endl;
	std::cout << sizeof(sockaddr_in6) << std::endl;
    std::cout << "Used CreateFileW" << std::endl;

	// Test networking
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup function failed with error: %d\n", iResult);
		return 1;
	}


	SOCKET sock;
	sock = socket(AF_INET,  // IPv4
		SOCK_STREAM, // TCP
		IPPROTO_TCP); // TCP

	sockaddr_in address_info;
	address_info.sin_family = AF_INET;
	address_info.sin_port = htons(PORT);
	 InetPtonW(AF_INET, L"127.0.0.1", &address_info.sin_addr.s_addr);

	int result = connect(sock,
		(sockaddr*) &address_info,
		sizeof(address_info));

	if (result == SOCKET_ERROR) {
		std::cout << "Socket failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		system("pause");
		return 1;
	} 

	send(sock, "Hello World", 12, 0);
	closesocket(sock);

	WSACleanup();
    system("pause");
}

