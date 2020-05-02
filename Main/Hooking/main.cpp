#include "graphics.h"
#include "hooking.h"
#include "hashmap.h"
#include "sha256.h"
#include "inject.h"
#define UNICODE 1

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	/*
	std::wcout << L"hey" << std::endl;
	wchar_t* key = (wchar_t*)L"ddddddsa";
	std::wstring value(L"dsadsa");
	char buf[1000] = { 0 };

	int ret = std::wcstombs(buf, key, sizeof(buf));
	if (ret == 1000) buf[999] = '\0';
	std::string hash_this(buf, 0, strlen(buf));
	
	std::wcout << key << std::endl;
	std::wcout << map->get_value(key) << std::endl;

	std::string _ = sha256((unsigned char*)buf, strlen(buf));
	std::wstring encoded(_.begin(), _.end());
	*/
	hash_map* map = new hash_map(10000);
	run_forever(hInstance, hPrevInstance, lpCmdLine, nCmdShow, map);

	/*
	
	*/
	return 0; 
}	
