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


/*
Create menus for selecting files / directory
-> SelectDir: Select directory to track
-> SelectFile: Select exe to launch and track
*/
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

LRESULT CALLBACK OnEvent(HWND wnd, UINT uMsg,
						WPARAM wParam, LPARAM lParam) {

	PIDStruct childPID;
	HMENU generalMenu, fileMenu, openMenu, processMenu;
	
	std::wstring path;
	switch (uMsg) {
		case WM_CLOSE:
			DestroyWindow(wnd);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case SELECT_DIR_MENU:
					// Hooks user chosen directory
					SelectDir(&path);
					hook_folder(path);
					break;
				case TYPE_DIR_MENU:
					// TODO: Implement manually typing directory (or not idk)
					MessageBox(NULL, L"Type dir", L"Type dir", 0);
					break;

				case SELECT_LAUNCH_PROCESS:
					// Hooking.exe -> Stub.exe -> Child.exe
					// Close Stub.exe and child.exe is not a child of hooking.exe anymore
					SelectFile(&path);

					// Named pipe for getting the process PID
					if (launch_process(path, &childPID) == FALSE) {
						MessageBoxW(NULL, L"Could not launch process due to an error in the launch_process function in inject.cpp", L"Launch process error", MB_ICONERROR);
					}

					wait_for_injection_and_resume(&childPID);
					break;

				case SELECT_EXISTING_PROCESS:
					// TODO: Allow selection of existing process
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	HWND original_window = generate_window(hInstance, WINDOW_TITLE, X, Y, WIDTH, HEIGHT);
	ShowWindow(original_window, nCmdShow);
	UpdateWindow(original_window);
	MSG msg;
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}