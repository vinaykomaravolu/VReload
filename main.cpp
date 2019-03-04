#include <iostream>
#include <filewatcher.h>
#include <thread>
#include <chrono>
#include <functional>
#include <fstream>


void createDirectoryHelper(fs::path path) {
	if (!fs::exists(path) || !fs::is_directory(path)) {
		fs::create_directory(path);
	}
}

void createFileHelper(fs::path path) {
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		ofstream newFile;
		cout << path.filename();
		newFile.open(path.string().c_str());
	}
}

int main() {
	fs::path currentDirectory("Test");
	fs::path includeDirectory(currentDirectory.string() + "/include");
	fs::path binDirectory(currentDirectory.string() + "/bin");
	fs::path libDirectory(currentDirectory.string() + "/lib");
	fs::path srcDirectory(currentDirectory.string() + "/src");
	fs::path makeFile(currentDirectory.string() + "/Makefile");

	createDirectoryHelper(includeDirectory);
	createDirectoryHelper(binDirectory);
	createDirectoryHelper(libDirectory);
	createDirectoryHelper(srcDirectory);
	createFileHelper(makeFile);

	FDWatcher::Watcher w;
	


	
	w.textEnableAll();

	this_thread::sleep_for(std::chrono::seconds(2));
	w.displayFileWatchers();
	while (true) {
		this_thread::sleep_for(std::chrono::seconds(50));
	}
	return 0;
}

/* Running Bat file example. Source: http://www.cplusplus.com/forum/general/102587/
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