#include <iostream>
#include <filewatcher.h>
#include <thread>



int main() {
	//temp file
	//TODO:: ADD USER INPUT
	string filePath;
	FileWatcher fw("C:/Users/Main/Desktop/Test");
	while (true) {
		this_thread::sleep_for(2s);
		fw.watch();
	}
	
	return 0;
}

/*
#include <windows.h>
#include <stdio.h>

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,
		"cmd /C  hello.bat",
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi )
		)
	{
		printf( "CreateProcess failed (%d)\n", GetLastError() );
		return FALSE;
	}
	WaitForSingleObject( pi.hProcess, INFINITE );
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return 0;
}
*/