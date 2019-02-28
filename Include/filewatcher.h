#pragma once
#include <iostream>
#include <chrono>
#include <filesystem>
#include <experimental/filesystem>
#include <thread>
#include <unordered_map>
#include <string>
#include <windows.h>
#include <vector>
#include <functional>


using namespace std;
namespace fs = experimental::filesystem;
using namespace std::chrono_literals;

struct Params {

};


class FileWatcher {
public:
	string pathWatch;
	thread fileWatcherThread;
	int delayTime; //In milliseconds
	bool fileWatcherThreadEnable = false;
	bool fileWatcherThreadCreated = false;
	function<int()> functionOnFileChange;
	unordered_map<string, time_t> fileWriteTimes;
	vector<fs::path> modifedFileDirectories;
	vector<fs::path> deletedFileDirectories;
	vector<fs::path> newFileDirectories;
	bool fileDirectoryModifiedFunctionEnable = false;
	bool fileDirectoryNewFunctionEnable = false;
	bool fileDirectoryDeleteFunctionEnable = false;
	bool fileDirectoryAllFunctionEnable = true;


	FileWatcher(string pathWatch, function<int()> functionOnFileChange ,int timeDelay) {
		this->pathWatch = pathWatch;
		this->functionOnFileChange = functionOnFileChange;
		this->delayTime = timeDelay;

		//Initial File Check
		for (auto &p : fs::recursive_directory_iterator(pathWatch)) {

			//Check and convert current paths' last write time 
			auto ftime = fs::last_write_time(p.path());
			time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			//Auto lastWriteTime =  std::asctime(std::localtime(&cftime));

			//Add path to unordered map
			fileWriteTimes.insert(make_pair(p.path().string(), cftime));
			
		}
	}

	void displayAllData() {
		cout << "**************************************************************************************" << endl;
		displayData(deletedFileDirectories, "Deleted");
		displayData(newFileDirectories, "New");
		displayData(modifedFileDirectories, "Modified");
		cout << "**************************************************************************************" << endl;
	}

	void displayAllPaths() {
		for (auto iter = fileWriteTimes.begin(); iter != fileWriteTimes.end(); ++iter) {
			cout << iter->first << endl;
		}
	}

	string getPathName() {
		return pathWatch;
	}

	void checkFilesForChange() {

		//Delay thread for processing
		this_thread::sleep_for(chrono::seconds(delayTime));

		while (fileWatcherThreadEnable) {
			// Temperarary data for file/directories write times, modified file/directories, deleted file/directories, and new file/directories
			unordered_map<string, time_t> tempfileWriteTimes;
			vector<fs::path> tempModifedFileDirectories;
			vector<fs::path> tempDeletedFileDirectories;
			vector<fs::path> tempNewFileDirectories;

			for (auto &p : fs::recursive_directory_iterator(pathWatch)) {
				fs::path currentPath = p;
				auto ftime = fs::last_write_time(currentPath);
				time_t currentLastWriteTime = decltype(ftime)::clock::to_time_t(ftime);

				tempfileWriteTimes.insert(make_pair(currentPath.string(), currentLastWriteTime));
				if (fileWriteTimes.count(currentPath.string()) == 0) { // New File/Directories
					tempNewFileDirectories.push_back(currentPath);
				}
				else {
					time_t previousLastWriteTime = fileWriteTimes[currentPath.string()];
					if (previousLastWriteTime != currentLastWriteTime) {
						tempModifedFileDirectories.push_back(currentPath);
					}
				}
				
			}
			for (const auto& p : fileWriteTimes) {
				if (tempfileWriteTimes.count(p.first) == 0) { //Deleted File/Directories
					tempDeletedFileDirectories.push_back(p.first);
				}	
			}

			deletedFileDirectories = tempDeletedFileDirectories;
			newFileDirectories = tempNewFileDirectories;
			modifedFileDirectories = tempModifedFileDirectories;
			fileWriteTimes = tempfileWriteTimes;

			// Check to see if file is created, deleted, or modified
			if (deletedFileDirectories.size() != 0 || newFileDirectories.size() != 0 || modifedFileDirectories.size() != 0 && fileDirectoryAllFunctionEnable) {
				if (functionOnFileChange) {
					createNewFunctionThreadD(functionOnFileChange);
				}
				else {//If no function on file change being called then just prints the file that has changed!
					thread functionThread = thread(&FileWatcher::displayAllData,this);
					functionThread.detach();
				}
			}
			else {
				if (deletedFileDirectories.size() != 0 && fileDirectoryDeleteFunctionEnable) {
					displayData(deletedFileDirectories, "Deleted");
				}

				if (newFileDirectories.size() != 0 && fileDirectoryDeleteFunctionEnable) {
					displayData(newFileDirectories, "New");
				}

				if (modifedFileDirectories.size() != 0 && fileDirectoryModifiedFunctionEnable) {
					displayData(modifedFileDirectories, "Modified");
				}
			}

			

			//Delay thread for processing
			this_thread::sleep_for(chrono::seconds(delayTime));
		}
		fileWatcherThreadCreated = false;
	}

	

	void execute() {
		if (fileWatcherThreadCreated == false) {
			fileWatcherThreadCreated = true;
			cout << "Watching File: " << pathWatch << endl;
			fileWatcherThreadEnable = true;
			fileWatcherThread = thread(&FileWatcher::checkFilesForChange, this);
			fileWatcherThread.detach();
		}
		else {
			cout << "Already Watching File: " << pathWatch << endl;
		}
	}

	void terminate() {
		fileWatcherThreadEnable = false;
		fileWatcherThreadCreated = false;
		cout << "Stopped Watching: " << pathWatch << endl;
	}
private:
	void displayData(vector<fs::path> ddir, string title = "") {
		if (title != "") {
			cout << title << ":" << endl;
		}
		else {
			cout << "Data:" << endl;
		}
		for (const auto& p : ddir) {
			cout << "\t" << p << endl;
		}
		cout << "\n";
	}


	void createNewFunctionThreadD(function<int()> functionOnFileChange) {
		thread functionThread = thread(functionOnFileChange);
		functionThread.detach();
	}

	void createNewFunctionThread(function<int()> functionOnFileChange) {
		thread functionThread = thread(functionOnFileChange);
		functionThread.join();
	}
};

//A file watcher manager
class Watcher {
public:
	vector<FileWatcher> fileWatchers;

	Watcher() {
		//Default Constructor
	}

	void watchFile(string pathWatch, function<int()> functionOnFileChange = NULL, int timeDelay = 1) {
		fileWatchers.push_back(FileWatcher(pathWatch, functionOnFileChange, timeDelay));
	}

	void displayFileWatchers() {
		cout << "Watching:" << endl;
		for (int i = 0; i < fileWatchers.size(); i++) {
			cout << "   [" << i << "]: " << fileWatchers[i].getPathName() << endl;
		}
	}

	void displayFileWatcher(int index) {
		if (index < 0 && index < fileWatchers.size()) {
			return;
		}
		cout << "[" << index << "]: " << fileWatchers[index].getPathName() << endl;
	}

	void executeAll() {
		for (int i = 0; i < fileWatchers.size(); i++) {
			fileWatchers[i].execute();
		}
	}

	void terminateAll() {
		for (int i = 0; i < fileWatchers.size(); i++) {
			fileWatchers[i].terminate();
		}
	}
};
