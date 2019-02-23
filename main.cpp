#include <iostream>
#include <filewatcher.h>
#include <thread>

using namespace std;

int main() {
	//temp file
	FileWatcher fw("C:/Users/Main/Desktop/CS 3rd year/CSC309");
	while (true) {
		this_thread::sleep_for(2s);
		fw.watch();
	}
	
	return 0;
}