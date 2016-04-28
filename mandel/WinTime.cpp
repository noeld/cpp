// WinTime.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include <tchar.h>

inline
uint64_t FToUInt(const FILETIME& f) {
	return (static_cast<uint64_t>(f.dwHighDateTime) << sizeof(DWORD)) + f.dwLowDateTime;
}


void _tmain(int argc, TCHAR *argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (argc != 2)
	{
		printf("Usage: %ws [cmdline]\n", argv[0]);
		return;
	}

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		argv[1],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	FILETIME creationTime, exitTime, kernelTime, userTime;

	GetProcessTimes(pi.hProcess, &creationTime, &exitTime, &kernelTime, &userTime);
	auto convertToSeconds = [](const FILETIME& time) -> double { return FToUInt(time) / 10000000.0; };
	_tprintf(_T("Kernel: %3.6f\n"), convertToSeconds(kernelTime));
	_tprintf(_T("User:   %3.6f\n"), convertToSeconds(userTime));
	_tprintf(_T("Real:   %3.6f\n"), (FToUInt(exitTime)-FToUInt(creationTime)) / 10000000.0);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}