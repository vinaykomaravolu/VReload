#include <iostream>
#include <filewatcher.h>
#include <thread>
#include <chrono>
#include <functional>
#include <fstream>

#define MAKEFILE "CC		:= g++\nC_FLAGS := -std=c++17 -Wall -Wextra\n\nBIN		:= bin\nSRC		:= src\nINCLUDE	:= include\nLIB		:= lib\n\nLIBRARIES	:=\n\nifeq ($(OS),Windows_NT)\nEXECUTABLE	:= main.exe\nelse\nEXECUTABLE	:= main\nendif\n\nall: $(BIN)/$(EXECUTABLE)\n\nclean:\n	$(RM) $(BIN)/$(EXECUTABLE)\n\nrun: all\n	./$(BIN)/$(EXECUTABLE)\n\n$(BIN)/$(EXECUTABLE): $(SRC)/*\n	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)"
#define SIMPLEMAINFILE "#include <iostream>\nusing namespace std;\n\nint main(){\n\n\n}"


void createDirectoryHelper(fs::path path) {
	if (!fs::exists(path) || !fs::is_directory(path)) {
		fs::create_directory(path);
	}
}

void createFileHelper(fs::path path,string textToAdd) {
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		ofstream newFile;
		newFile.open(path.string().c_str());
		newFile << textToAdd << endl;
		newFile.close();
	}
}

int makeProject() {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL,
		(LPSTR)"cmd /C  run.bat",
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		)
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

int main() {
	fs::path currentDirectory = fs::current_path();
	fs::path includeDirectory(currentDirectory.string() + "\\include");
	fs::path binDirectory(currentDirectory.string() + "\\bin");
	fs::path libDirectory(currentDirectory.string() + "\\lib");
	fs::path srcDirectory(currentDirectory.string() + "\\src");
	fs::path makeFile(currentDirectory.string() + "\\Makefile");
	fs::path mainFile(currentDirectory.string() + "\\src/main.cpp");
	
	//FOR WINDOWS ONLY
	fs::path batFILE(currentDirectory.string() + "\\run.bat");

	createDirectoryHelper(includeDirectory);
	createDirectoryHelper(binDirectory);
	createDirectoryHelper(libDirectory);
	createDirectoryHelper(srcDirectory);
	createFileHelper(makeFile, MAKEFILE);
	createFileHelper(mainFile, SIMPLEMAINFILE);
	createFileHelper(batFILE, "mingw32-make\nbin\\main.exe");

	FDWatcher::Watcher w;
	w.watchFile(currentDirectory.string(), makeProject);

	w.textEnableAll();

	while (true) {
		this_thread::sleep_for(std::chrono::seconds(50));
	}
	return 0;
}

