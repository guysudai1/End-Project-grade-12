#include <Windows.h>
#include <shobjidl.h> 
#include "graphics.h"
#include "hashmap.h"
#include "inject.h"

#define WIDTH   500
#define HEIGHT  500
#define X		500
#define Y		500
#define WINDOW_TITLE L"my_window"

#define SELECT_DIR_MENU 1
#define TYPE_DIR_MENU 2
#define SELECT_LAUNCH_PROCESS 3
#define SELECT_EXISTING_PROCESS 4

hash_map* my_map;

class CReceiver {
public:
	void InputBox(std::string nValue) {
		MessageBoxA(NULL, "Handler", nValue.c_str(), 0);
	}
};

VOID StartProcess(std::wstring application_name, std::wstring child_process)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(application_name.c_str(),   // the path
		(LPWSTR)((application_name + L"\"" + child_process + L"\"").c_str()),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void SelectDir(std::wstring* path) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;
		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			/*
			COMDLG_FILTERSPEC rgSpec[] =
			{
				{ L"Select folder ", L"*\/" },
				{ L"All types ", L"*.*"}
			};
			pFileOpen->SetFileTypes(2, rgSpec);
			*/
			pFileOpen->SetOptions(FOS_PICKFOLDERS);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					
					if (SUCCEEDED(hr))
					{
						*path = pszFilePath;
						CoTaskMemFree(pszFilePath);
					
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

}

void SelectFile(std::wstring* path) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;
		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			COMDLG_FILTERSPEC rgSpec[] =
			{
				{ L"Select executable", L"*.exe" },
				{ L"All types ", L"*.*"}
			};
			pFileOpen->SetFileTypes(2, rgSpec);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					
					if (SUCCEEDED(hr))
					{
						*path = pszFilePath;
						CoTaskMemFree(pszFilePath);
					
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

}

LRESULT CALLBACK OnEvent(HWND wnd,
						UINT uMsg,
						WPARAM wParam,
						LPARAM lParam) {
	HANDLE pipe;
	HMENU generalMenu, fileMenu, openMenu, openDir, processMenu, launchProcess, existingProcess;
	DWORD low, read_amount, child_pid;
	bool connected = false;
	char buf[1024];
	std::wstring path;
	switch (uMsg) {
		case WM_CLOSE:
			// MessageBoxW(wnd, L"Closing window...", L"Close", 0);
			DestroyWindow(wnd);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case SELECT_DIR_MENU:
					SelectDir(&path);
					// capture_folder(path, my_map, true);
					hook_folder(path);
					break;
				case TYPE_DIR_MENU:
					MessageBox(NULL, L"Type dir", L"Type dir", 0);
					break;

				case SELECT_LAUNCH_PROCESS:
					// Hooking.exe -> Stub.exe -> Child.exe
					// Close Stub.exe and child.exe is not a child of hooking.exe anymore
					SelectFile(&path);
					// Named pipe for getting the process PID
					pipe = CreateNamedPipeA("\\\\.\\pipe\\get_child_pid",
						PIPE_ACCESS_INBOUND,
						PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
						1,
						1024,
						1024,
						NMPWAIT_USE_DEFAULT_WAIT,
						NULL);
					
					if (pipe == INVALID_HANDLE_VALUE)
					{
						MessageBoxA(NULL, "CreateNamedPipe failed", std::to_string(GetLastError()).c_str(), MB_ICONWARNING);
						return -1;
					}

					StartProcess(L"C:\\Users\\sudai\\source\\repos\\End-Project-grade-12\\Stub\\stub.exe", path);

					while (!connected && pipe != INVALID_HANDLE_VALUE)
					{
						if (ConnectNamedPipe(pipe, NULL) != FALSE)   // wait for someone to connect to the pipe
						{
							while (ReadFile(pipe, buf, sizeof(buf) - 1, &read_amount, NULL) != FALSE)
							{
								/* add terminating zero */
								buf[read_amount] = '\0';
								connected = true;
							}
						}

						DisconnectNamedPipe(pipe);
					}

					
					if (std::string(buf).compare("Error") == 0) {
						MessageBoxA(NULL, "Launching process failed", std::to_string(GetLastError()).c_str(), MB_ICONWARNING);
						return -1;
					}

					child_pid = std::stoi(std::string(buf));
					inject_to_process(child_pid, L"C:\\Users\\sudai\\source\\repos\\End-Project-grade-12\\DLL\\file_hooker.dll");
					MessageBoxA(NULL, "Child PID", std::string(buf).c_str(), 0);
					break;

				case SELECT_EXISTING_PROCESS:
					break;
			}
			break;
		case WM_CREATE:
			/*
			Menu tree:
			-> File
			 -> Open
			   -> Select Directory (SELECT_DIR_MENU)
			   -> Type Directory Path (TYPE_DIR_MENU)
			-> Process
			 -> Launch (SELECT_LAUNCH_PROCESS)
			 -> Process List (SELECT_EXISTING_PROCESS)
			*/
			generalMenu = CreateMenu();

			fileMenu = CreateMenu();
			openMenu = CreatePopupMenu();

			processMenu = CreateMenu();
			/*	MENUINFO mi = { 0 };
			mi.cbSize = sizeof(mi);
			mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
			mi.hbrBack = reinterpret_cast<HBRUSH>(COLOR_MENU);
			SetMenuInfo(menu, &mi);*/

			AppendMenu(generalMenu, MF_STRING | MF_POPUP, (UINT_PTR)fileMenu, L"File");
			AppendMenu(generalMenu, MF_STRING | MF_POPUP, (UINT_PTR)processMenu, L"Process");
			AppendMenu(fileMenu, MF_STRING | MF_POPUP, (UINT_PTR)openMenu, L"Open");

			AppendMenu(processMenu, MF_STRING, SELECT_LAUNCH_PROCESS, L"Launch new process");
			AppendMenu(processMenu, MF_STRING, SELECT_EXISTING_PROCESS, L"Existing process");

			AppendMenu(openMenu, MF_STRING , SELECT_DIR_MENU, L"Select Directory");
			AppendMenu(openMenu, MF_STRING, TYPE_DIR_MENU, L"Type Directory Path");

			SetMenu(wnd, generalMenu);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(wnd, uMsg, wParam, lParam);
	}
}

HWND generate_window(HINSTANCE hInstance,
					std::wstring name, 
					int x, int y, 
					int width, int height) {

	std::wstring className(L"class");
	WNDCLASSW WindowClass;
	WindowClass.style = 0;
	WindowClass.lpfnWndProc = &OnEvent;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = NULL;
	WindowClass.hCursor = 0;
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = className.c_str();
	if (!RegisterClass(&WindowClass)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	DWORD dwStyle =	WS_OVERLAPPEDWINDOW;
	// Let's create the main window
	HWND hWnd = CreateWindowEx(0,   // dwExStyle
		className.c_str(),			 // lpClassName
		name.c_str(),				 // lpWindowName
		dwStyle,					 // dwStyle
		x, y,						 // x,y
		width, height,				 // width height
		NULL,						 // Parent window
		NULL,						 // hMenu
		hInstance,					 // Hinstance
		NULL						 // additional data
	);

	return hWnd;
}

// void create_menu(sf::RenderWindow* wnd) {
//	HWND windows_handle = wnd->getSystemHandle();
// }

void run_forever(HINSTANCE hInstance, 
				HINSTANCE hPrevInstance,
				LPSTR lpCmdLine, 
				int nCmdShow,
				hash_map* map) {
	my_map = map;

	HWND original_window = generate_window(hInstance, WINDOW_TITLE, X, Y, WIDTH, HEIGHT);
	ShowWindow(original_window, nCmdShow);
	UpdateWindow(original_window);
	MSG msg;
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

/*	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
				case sf::Event::Closed:
					window.close();
					break;
					
		}

		window.clear();
		// window.draw(shape);
		window.display();
	} */
}