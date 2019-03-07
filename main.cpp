#include <iostream>
#include <filewatcher.h>
#include <thread>
#include <chrono>
#include <functional>
#include <fstream>

#define MAKEFILE "CC		:= g++\nC_FLAGS := -std=c++17 -Wall -Wextra\n\nBIN		:= bin\nSRC		:= src\nINCLUDE	:= include\nLIB		:= lib\n\nLIBRARIES	:=\n\nifeq ($(OS),Windows_NT)\nEXECUTABLE	:= main.exe\nelse\nEXECUTABLE	:= main\nendif\n\nall: $(BIN)/$(EXECUTABLE)\n\nclean:\n	$(RM) $(BIN)/$(EXECUTABLE)\n\nrun: all\n	./$(BIN)/$(EXECUTABLE)\n\n$(BIN)/$(EXECUTABLE): $(SRC)/*\n	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)"
#define SIMPLEMAINFILE "#include <iostream>\nusing namespace std;\n\nint main(){\n\n\n}"

void clearScreen()
{
	std::system("cls");
}


// Process Currently Running
bool batchProcessRunning = false;
bool executableProcessRunning = false;
HANDLE hBatchProcess = NULL;
HANDLE hExecutableProcess = NULL;

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
	if (!batchProcessRunning) {
		clearScreen(); // RISKY/UNSTABLE

		// If executable process is still running then close the current one
		if (hExecutableProcess) {
			TerminateProcess(hExecutableProcess, 0);
		}

		// Run the batch file
		batchProcessRunning = true;
		STARTUPINFO siBat;
		PROCESS_INFORMATION piBat;

		ZeroMemory(&siBat, sizeof(siBat));
		siBat.cb = sizeof(siBat);
		ZeroMemory(&piBat, sizeof(piBat));
		if (!CreateProcess(NULL,
			(LPSTR)"run.bat",
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&siBat,
			&piBat)
			)
		{
			printf("CreateProcess failed (%d)\n", GetLastError());
			return FALSE;
		}
		WaitForSingleObject(piBat.hProcess, INFINITE);
		CloseHandle(piBat.hProcess);
		CloseHandle(piBat.hThread);
		batchProcessRunning = false;


		// Executable Process
		STARTUPINFO siExec;
		PROCESS_INFORMATION piExec;

		ZeroMemory(&siExec, sizeof(siExec));
		siExec.cb = sizeof(siExec);
		ZeroMemory(&piExec, sizeof(piExec));

		// Checking to see if the exe is created or failed
		fs::path currentDirectory = fs::current_path();
		fs::path exeFile(currentDirectory.string() + "\\bin\\main.exe");
		if (fs::exists(exeFile)) {
			if (!CreateProcess(NULL,
				(LPSTR)"bin\\main.exe",
				NULL,
				NULL,
				FALSE,
				0,
				NULL,
				NULL,
				&siExec,
				&piExec)
				)
			{
				printf("CreateProcess failed (%d)\n", GetLastError());
				return FALSE;
			}
			hExecutableProcess = piExec.hProcess;
			WaitForSingleObject(piExec.hProcess, INFINITE);
			hExecutableProcess = NULL;
			CloseHandle(piExec.hProcess);
			CloseHandle(piExec.hThread);
		}
	}
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
	createFileHelper(batFILE,"mingw32-make\nIf %ERRORLEVEL% NEQ 0 (\n\tdel \"bin\\main.exe\"\n)");

	FDWatcher::Watcher w;
	w.watchFile(currentDirectory.string() + "\\src", makeProject);
	w.watchFile(currentDirectory.string() + "\\include", makeProject);
	w.watchFile(currentDirectory.string() + "\\lib", makeProject);

	w.textEnableAll();

	while (true) {
		this_thread::sleep_for(std::chrono::seconds(50));
	}
	return 0;
}

