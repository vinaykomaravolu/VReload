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

	void checkFilesForChange() {

		//Delay thread for processing
		this_thread::sleep_for(chrono::seconds(delayTime));

		while (fileWatcherThreadEnable) {
			//To account for removed or name changed file
			unordered_map<string, time_t> tempfileWriteTimes;
			for (auto &p : fs::recursive_directory_iterator(pathWatch)) {
				fs::path currentPath = p.path();
				auto ftime = fs::last_write_time(currentPath);
				time_t currentLastWriteTime = decltype(ftime)::clock::to_time_t(ftime);
				//check if path exists in map
				//if path does not exist then it is created or modified
				if (fileWriteTimes.count(currentPath.string()) == 0) {
					tempfileWriteTimes.insert(make_pair(currentPath.string(), currentLastWriteTime));
					cout << currentPath << endl;
				}
				time_t previousLastWriteTime = fileWriteTimes[currentPath.string()];

				//if the file has been changed then execute function and return from the function
				//also update path last write time
				if (previousLastWriteTime != currentLastWriteTime) {
					tempfileWriteTimes.insert(make_pair(currentPath.string(), currentLastWriteTime));
					if (functionOnFileChange) {
						thread functionThread = thread(this->functionOnFileChange);
						functionThread.detach();
					}
					else {//If no function on file change being called then just prints the file that has changed!
						cout << "File Changed: " << currentPath.string();
					}
				}
				else {
					tempfileWriteTimes.insert(make_pair(currentPath.string(), previousLastWriteTime));
				}
			}
			fileWriteTimes = tempfileWriteTimes;

			//Delay thread for processing
			this_thread::sleep_for(chrono::seconds(delayTime));
		}
		fileWatcherThreadCreated = false;
	}

	void displayAllPaths() {
		for (auto iter = fileWriteTimes.begin(); iter != fileWriteTimes.end(); ++iter) {
			cout << iter->first << endl;
		}
	}

	string getPathName() {
		return pathWatch;
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
		cout << "Stopped Watching: " << pathWatch << endl;
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
		fileWatchers.push_back(FileWatcher(pathWatch, functionOnFileChange,timeDelay));
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
