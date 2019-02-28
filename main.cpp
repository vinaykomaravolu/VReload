#include <iostream>
#include <filewatcher.h>
#include <thread>
#include <chrono>
#include <functional>


//Thread Testing
class A {
public:
	int a;
	bool terminateThread;
	std::thread t1;
	A(int x) {
		terminateThread = true;
		a = x;
	}

	void printInf() {
		while (!terminateThread) {
			this_thread::sleep_for(std::chrono::milliseconds(100));
			std::cout << a << std::endl;
		}
	}

	void execute() {
		terminateThread = false;
		std::thread(&A::printInf,this);
	}

	void destroy() {
		terminateThread = true;
	}
};

int a() {
	while (true) {
		this_thread::sleep_for(std::chrono::seconds(2));
		cout << this_thread::get_id() << endl;
	}
	return 0;
};




void test(std::function<int()> b) {
	if (b) {
		b();
	}
};

int main() {
	/*string filePath;
	FileWatcher fw("C:\\Users\\Main\\Desktop\\Project\\Reload\\VReload\\Test",1);
	FileWatcher fw2("C:\\Users\\Main\\Desktop\\Test",1);
	A b(10);
	A c(2);
	
	fw.execute();
	this_thread::sleep_for(std::chrono::seconds(2));

	fw.execute();
	this_thread::sleep_for(std::chrono::seconds(2));
	fw2.execute();

	this_thread::sleep_for(std::chrono::seconds(2));
	fw.terminate();
	this_thread::sleep_for(std::chrono::seconds(2));
	fw.execute();

	this_thread::sleep_for(std::chrono::seconds(50));
	*/
	
	Watcher w;
	w.watchFile("C:\\Users\\Main\\Desktop\\Project\\Reload\\VReload\\Test");
	w.watchFile("C:\\Users\\Main\\Desktop\\Test");

	

	this_thread::sleep_for(std::chrono::seconds(2));
	//w.displayFileWatchers();

	this_thread::sleep_for(std::chrono::seconds(50));
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