/*	Author		:	Alex Hester
	Date		:	Jul 5, 2023
	Purpose		:	A simple GUI for running DLLInjector and storing app IDs
	Filename	:	automush.cpp	*/

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <fstream>
#include <string>
#include <tlhelp32.h>
#define IDI_AUTOMUSH 101

HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

HWND hwndMain;
HWND hwndAutomush;

// Function for error detection
bool isProcessRunning(const std::string& processName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		
		if (!Process32First(hSnapshot, &pe32))
		{
			CloseHandle(hSnapshot);
			return false;
		}
		
		do
		{
			if (std::string(pe32.szExeFile) == processName)
			{
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Process32Next(hSnapshot, &pe32));
		
		CloseHandle(hSnapshot);
		return false;
		}

// Function for running DLLInjector
void run()
{
	// Call function to check if DLLInjector is already running
	if (isProcessRunning("DLLInjector.exe"))
	{
		MessageBox(NULL,"DLLInjector is already running.", "Error!", MB_OK);
		return;
	}

	// Call function to check if Steam is already running
	if (isProcessRunning("steam.exe"))
	{
		MessageBox(NULL,"Close Steam before running Automush.", "Error!", MB_OK);
		return;
	}

	// Open automush.txt
	std::ifstream inputFile("automush.txt");

	// Check if automush.txt was successfully opened
	if (inputFile.is_open())
	{
		std::string line;
		int lineCount = 0;
		// Count total lines in automush.txt
		while (getline(inputFile, line))
		{
			lineCount++;

			// Error message
			if(lineCount > 149)
			{
				MessageBox(NULL,"You must have less than 149 app ids in automush.txt", "Error!", MB_OK);
			}
		}

		//Get path to DLLInjector from automush.ini
		std::string applicationPath;
		std::ifstream configFile("automush.ini");
		if (configFile.is_open())
		{
			getline(configFile, applicationPath);
			// Close the file after reading
			configFile.close();
		}

		// Error message
		else
		{
			MessageBox(NULL,"Failed to open automush.ini", "Error!", MB_OK);
		}

		// Open DLLInjector
		ShellExecute(NULL, "open", applicationPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		
		// Wait for DLLInjector to open before continuing
		HWND windowHandle = nullptr;
		while (windowHandle == nullptr)
		{
			windowHandle = FindWindowA(nullptr, "GreenLuma 2023");
			Sleep(500); // Wait for 1 second before checking again
		}

		// Input the left arrow key
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = VK_LEFT;
		input.ki.dwFlags = 0;
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		SendInput(1, &input, sizeof(INPUT));

		// Wait
		Sleep(50);

		// Input the enter key
		input.ki.wVk = VK_RETURN;
		SendInput(1, &input, sizeof(INPUT));

		// Wait
		Sleep(250);

		// Input the total games to add
		std::string lineCountStr = std::to_string(lineCount);
		for (int i = 0; i < lineCountStr.length(); i++)
		{
			char c = lineCountStr[i];
			input.ki.wVk = c;
			SendInput(1, &input, sizeof(INPUT));
		}

		// Input the enter key
		input.ki.wVk = VK_RETURN;
		SendInput(1, &input, sizeof(INPUT));

		// Clear the end-of-file flag
		inputFile.clear();
		// Move the file pointer to the beginning
		inputFile.seekg(0, std::ios::beg);
		// Read automush.txt line by line
		while (getline(inputFile, line))
		{
			// Input each line into DLLInjector
			for (int i = 0; i < line.length(); i++)
			{
				char c = line[i];
				input.ki.wVk = c;
				SendInput(1, &input, sizeof(INPUT));
			}
			
			// Input the enter key
			input.ki.wVk = VK_RETURN;
			SendInput(1, &input, sizeof(INPUT));
		}

		// Close the file after reading
		inputFile.close();
	}

	// Error message
	else
	{
		MessageBox(NULL,"Failed to find automush.txt", "Error!", MB_OK);
	}
}

// Function for handling button presses
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				// Case for pressing run
				case 1:
					run();
					PostQuitMessage(0);
					break;

				// Case for pressing Add APP ID
				case 2:
					ShellExecute(NULL, "open", "automush.txt", NULL, NULL, SW_SHOWNORMAL);
					Sleep(250);
					break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
return 0;
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Register window class
	WNDCLASSEX wcMain = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "MyWindowClass", NULL };
	wcMain.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOMUSH));
	wcMain.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOMUSH));
	RegisterClassEx(&wcMain);

	// Get screen resolution
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Calculate window position
	int windowWidth = 245;
	int windowHeight = 90;
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	// Create window
	hwndMain = CreateWindow(wcMain.lpszClassName, "Automush", WS_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, windowX, windowY, windowWidth, windowHeight, NULL, NULL, wcMain.hInstance, NULL);

	// Create buttons in window
	HWND button1 = CreateWindow("BUTTON", "Run", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 30, hwndMain, (HMENU)1, wcMain.hInstance, NULL);
	HWND button2 = CreateWindow("BUTTON", "Add APP ID", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 10, 100, 30, hwndMain, (HMENU)2, wcMain.hInstance, NULL);

	SendMessage(hwndMain, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(button1, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(button2, WM_SETFONT, (WPARAM)hFont, TRUE);
	
	// Show window
	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	// Message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
return 0;
}