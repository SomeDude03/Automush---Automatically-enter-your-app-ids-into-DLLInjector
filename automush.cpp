/*	Author		:	Alex Hester
	Date		:	Jun 28, 2023
	Purpose		:	To take a list of steam app ids and input them into DLLInjector
	Filename	:	automush.cpp	*/

#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
using namespace std;

int main()
{
	
	// Open automush
	ifstream inputFile("automush.txt"); // Open the text file for reading
	if (inputFile.is_open()) // Check if the file was successfully opened
	{
		string line;
		int lineCount = 0;
		// Count total lines in automush.txt
		while (getline(inputFile, line))
		{
			lineCount++;
			if(lineCount > 149)
			{
				MessageBox(NULL,"You must have less than 149 app ids in automush.txt", "Error!", MB_OK);
				return 0;
			}
		}
		
		//Get path to DLLInjector from automush.ini
		string applicationPath;
		ifstream configFile("automush.ini");
		if (configFile.is_open())
		{
			getline(configFile, applicationPath);
			// Close the file after reading
			configFile.close();
		}
		else
		{
			MessageBox(NULL,"Failed to open automush.ini", "Error!", MB_OK);
			return 0;
		}
		
		// Open DLLInjector
		ShellExecute(NULL, "open", applicationPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);

		// Wait
		Sleep(500);

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
		string lineCountStr = to_string(lineCount);
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
		inputFile.seekg(0, ios::beg);
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
			
			// Wait
			// Sleep(1);
		}
		
		// Close the file after reading
		inputFile.close();
	}
	
	else
	{
		MessageBox(NULL,"Failed to open automush.txt", "Error!", MB_OK);
	}

return 0;
}
