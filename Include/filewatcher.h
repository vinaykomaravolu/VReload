#pragma once
#include <iostream>
#include <chrono>
#include <filesystem>
#include <experimental/filesystem>
#include <thread>
#include <unordered_map>
#include <string>
#include <windows.h>


using namespace std;
namespace fs = experimental::filesystem;
using namespace std::chrono_literals;
class FileWatcher {
public:
	string pathWatch;
	unordered_map<string, time_t> fileWriteTimes;

	FileWatcher(string pathWatch) {
		this->pathWatch = pathWatch;
		for (auto &p : fs::recursive_directory_iterator(pathWatch)) {

			//Check and convert current paths' last write time 
			auto ftime = fs::last_write_time(p.path());
			time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			//auto lastWriteTime =  std::asctime(std::localtime(&cftime));

			//add path to unordered map
			fileWriteTimes.insert(make_pair(p.path().string(), cftime));
			
		}
		cout << "INIT" << endl;
	}

	void watch() {
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

			//if the file has been changed then print the path and return from the function
			//also update path last write time
			if (previousLastWriteTime != currentLastWriteTime) {
				tempfileWriteTimes.insert(make_pair(currentPath.string(), currentLastWriteTime));
				cout << currentPath << endl;
				//___________________________________________________TEST
				string command = "/C g++ -o main " + currentPath.string();
				cout << command;
				ShellExecute(0, "open", "cmd.exe", command.c_str(), 0, SW_HIDE);

				//___________________________________________________TEST
			}
			else {
				tempfileWriteTimes.insert(make_pair(currentPath.string(), previousLastWriteTime));
			}
		}
		fileWriteTimes = tempfileWriteTimes;
	}

	void displayAllPaths() {
		for (auto iter = fileWriteTimes.begin(); iter != fileWriteTimes.end(); ++iter) {
			cout << iter->first << endl;
		}
	}
};