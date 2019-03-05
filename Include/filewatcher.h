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
namespace FDWatcher
{
	struct params {
		// Create your own custom parameters
	};

class FileWatcher
{
  public:
	FileWatcher(string pathWatch, function<int()> functionOnFileChange, int timeDelay)
	{
		this->pathWatch = pathWatch;
		this->functionOnFileChange = functionOnFileChange;
		this->delayTime = timeDelay;

		//Initial File Check
		for (auto &p : fs::recursive_directory_iterator(pathWatch))
		{

			//Check and convert current paths' last write time
			auto ftime = fs::last_write_time(p.path());
			time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			//Auto lastWriteTime =  std::asctime(std::localtime(&cftime));

			//Add path to unordered map
			fileWriteTimes.insert(make_pair(p.path().string(), cftime));
		}
	}

	void displayAllData()
	{
		cout << "**************************************************************************************" << endl;
		displayData(deletedFileDirectories, "Deleted");
		displayData(newFileDirectories, "New");
		displayData(modifedFileDirectories, "Modified");
		cout << "**************************************************************************************" << endl;
	}

	void displayAllPaths()
	{
		for (auto iter = fileWriteTimes.begin(); iter != fileWriteTimes.end(); ++iter)
		{
			cout << iter->first << endl;
		}
	}

	string getPathName()
	{
		return pathWatch;
	}

	void onAllChangeFunction(function<int()> function)
	{
		functionOnFileChange = function;
	}

	void onDeleteFunction(function<int()> function)
	{
		functionOnDeleteFileChange = function;
	}

	void onModifiedFunction(function<int()> function)
	{
		functionOnModifiedFileChange = function;
	}

	void onNewFunction(function<int()> function)
	{
		functionOnNewFileChange = function;
	}

	void onAllChangeFunctionEnable()
	{
		fileDirectoryAllFunctionEnable = true;
		fileDirectoryModifiedFunctionEnable = false;
		fileDirectoryNewFunctionEnable = false;
		fileDirectoryDeleteFunctionEnable = false;
	}

	void onDeleteFunctionEnable()
	{
		fileDirectoryAllFunctionEnable = false;
		fileDirectoryDeleteFunctionEnable = true;
	}

	void onNewFunctionEnable()
	{
		fileDirectoryAllFunctionEnable = false;
		fileDirectoryNewFunctionEnable = true;
	}

	void onModifiedFunctionEnable()
	{
		fileDirectoryAllFunctionEnable = false;
		fileDirectoryModifiedFunctionEnable = true;
	}

	void execute()
	{
		if (!fileWatcherThreadCreated)
		{
			fileWatcherThreadCreated = true;
			fileWatcherThreadEnable = true;
			if (enableFileWatcherText)
			{
				cout << "Watching File: " << pathWatch << endl;
			}
			fileWatcherThread = thread(&FileWatcher::checkFilesForChange, this);
			fileWatcherThread.detach();
		}
		else
		{
			if (enableFileWatcherText)
			{
				cout << "Already Watching File: " << pathWatch << endl;
			}
		}
	}

	void terminate()
	{
		if (fileWatcherThreadCreated)
		{
			fileWatcherThreadEnable = false;
			fileWatcherThreadCreated = false;
			if (enableFileWatcherText)
			{
				cout << "Stopped Watching: " << pathWatch << endl;
			}
		}
		else
		{
			if (enableFileWatcherText)
			{
				cout << "Already Stopped Watching: " << pathWatch << endl;
			}
		}
	}

	void textEnable(bool enable = true)
	{
		enableFileWatcherText = enable;
	}

  private:
	string pathWatch;
	thread fileWatcherThread;
	int delayTime; //In milliseconds
	bool fileWatcherThreadEnable = false;
	bool fileWatcherThreadCreated = false;
	function<int()> functionOnFileChange;
	function<int()> functionOnDeleteFileChange;
	function<int()> functionOnNewFileChange;
	function<int()> functionOnModifiedFileChange;
	unordered_map<string, time_t> fileWriteTimes;
	vector<fs::path> modifedFileDirectories;
	vector<fs::path> deletedFileDirectories;
	vector<fs::path> newFileDirectories;
	bool fileDirectoryModifiedFunctionEnable = false;
	bool fileDirectoryNewFunctionEnable = false;
	bool fileDirectoryDeleteFunctionEnable = false;
	bool fileDirectoryAllFunctionEnable = true;
	bool enableFileWatcherText = false;

	void displayData(vector<fs::path> ddir, string title = "")
	{
		if (title != "")
		{
			cout << title << ":" << endl;
		}
		else
		{
			cout << "Data:" << endl;
		}
		for (const auto &p : ddir)
		{
			cout << "\t" << p << endl;
		}
		cout << "\n";
	}

	void createNewFunctionThreadD(function<int()> functionOnFileChange)
	{
		thread functionThread = thread(functionOnFileChange);
		functionThread.detach();
	}

	void createNewFunctionThread(function<int()> functionOnFileChange)
	{
		thread functionThread = thread(functionOnFileChange);
		functionThread.join();
	}

	// ALways runs on seperate thread
	void checkFilesForChange()
	{

		//Delay thread for processing
		//this_thread::sleep_for(chrono::seconds(delayTime));

		while (fileWatcherThreadEnable)
		{
			// Temperarary data for file/directories write times, modified file/directories, deleted file/directories, and new file/directories
			unordered_map<string, time_t> tempfileWriteTimes;
			vector<fs::path> tempModifedFileDirectories;
			vector<fs::path> tempDeletedFileDirectories;
			vector<fs::path> tempNewFileDirectories;

			for (auto &p : fs::recursive_directory_iterator(pathWatch))
			{
				fs::path currentPath = p;
				auto ftime = fs::last_write_time(currentPath);
				time_t currentLastWriteTime = decltype(ftime)::clock::to_time_t(ftime);

				tempfileWriteTimes.insert(make_pair(currentPath.string(), currentLastWriteTime));
				if (fileWriteTimes.count(currentPath.string()) == 0)
				{ // New File/Directories
					tempNewFileDirectories.push_back(currentPath);
				}
				else
				{
					time_t previousLastWriteTime = fileWriteTimes[currentPath.string()];
					if (previousLastWriteTime != currentLastWriteTime)
					{
						tempModifedFileDirectories.push_back(currentPath);
					}
				}
			}
			for (const auto &p : fileWriteTimes)
			{
				if (tempfileWriteTimes.count(p.first) == 0)
				{ //Deleted File/Directories
					tempDeletedFileDirectories.push_back(p.first);
				}
			}

			deletedFileDirectories = tempDeletedFileDirectories;
			newFileDirectories = tempNewFileDirectories;
			modifedFileDirectories = tempModifedFileDirectories;
			fileWriteTimes = tempfileWriteTimes;

			// Check to see if file is created, deleted, or modified
			if (fileDirectoryAllFunctionEnable && deletedFileDirectories.size() != 0 || newFileDirectories.size() != 0 || modifedFileDirectories.size() != 0)
			{
				if (functionOnFileChange)
				{
					createNewFunctionThreadD(functionOnFileChange);
				}
				else
				{ //If no function on file change being called then just prints the file that has changed!
					if (enableFileWatcherText)
					{
						thread functionThread = thread(&FileWatcher::displayAllData, this);
						functionThread.detach();
					}
				}
			}
			else
			{
				if (deletedFileDirectories.size() != 0 && fileDirectoryDeleteFunctionEnable)
				{
					if (functionOnDeleteFileChange)
					{
						createNewFunctionThreadD(functionOnDeleteFileChange);
					}
					else
					{
						if (enableFileWatcherText)
						{
							thread functionThread = thread(&FileWatcher::displayData, this, deletedFileDirectories, "Deleted");
							functionThread.detach();
						}
					}
				}

				if (newFileDirectories.size() != 0 && fileDirectoryDeleteFunctionEnable)
				{
					if (functionOnNewFileChange)
					{
						createNewFunctionThreadD(functionOnNewFileChange);
					}
					else
					{
						if (enableFileWatcherText)
						{
							thread functionThread = thread(&FileWatcher::displayData, this, newFileDirectories, "New");
							functionThread.detach();
						}
					}
				}

				if (modifedFileDirectories.size() != 0 && fileDirectoryModifiedFunctionEnable)
				{
					if (functionOnModifiedFileChange)
					{
						createNewFunctionThreadD(functionOnModifiedFileChange);
					}
					else
					{
						if (enableFileWatcherText)
						{
							thread functionThread = thread(&FileWatcher::displayData, this, modifedFileDirectories, "Modified");
							functionThread.detach();
						}
					}
				}
			}

			//Delay thread for processing
			this_thread::sleep_for(chrono::seconds(delayTime));
		}
	}

	// Personalized Functions
};

//A file watcher manager
class Watcher
{
  public:

	Watcher()
	{
		//Default Constructor
	}

	void watchFile(string pathWatch, function<int()> functionOnFileChange = NULL, int timeDelay = 1)
	{
		/*thread w = thread(&Watcher::_watchFile, this, pathWatch, functionOnFileChange, timeDelay);
			w.detach();*/
		fileWatchers.push_back(new FileWatcher(pathWatch, functionOnFileChange, timeDelay));
		fileWatchers[fileWatchers.size() - 1]->execute();
	}



	void displayFileWatchers()
	{
		cout << "Watching:" << endl;
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			cout << "   [" << i << "]: " << fileWatchers[i]->getPathName() << endl;
		}
	}

	void displayFileWatcher(int index)
	{
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		cout << "[" << index << "]: " << fileWatchers[index]->getPathName() << endl;
	}

	void watcherOnAllChangeFunction(int index, function<int()> function)
	{
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onAllChangeFunction(function);
	}

	void watcherOnAllChangeFunction(string path, function<int()> function)
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onNewFunction(function);
				break;
			}
		}
	}

	void watcherOnNewFunction(int index, function<int()> function)
	{
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onAllChangeFunction(function);
	}

	void watcherOnNewFunction(string path, function<int()> function)
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onNewFunction(function);
				break;
			}
		}
	}

	void watcherOnDeleteFunction(int index, function<int()> function)
	{
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onDeleteFunction(function);
	}

	void watcherOnDeleteFunction(string path, function<int()> function)
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onDeleteFunction(function);
				break;
			}
		}
	}

	void watcherOnModifiedFunction(int index, function<int()> function)
	{
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onModifiedFunction(function);
	}

	void watcherOnModifiedFunction(string path, function<int()> function)
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onModifiedFunction(function);
				break;
			}
		}
	}

	void watcherOnAllChangeFunctionEnable(int index) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onAllChangeFunctionEnable();
	}

	void watcherOnAllChangeFunctionEnable(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onAllChangeFunctionEnable();
				break;
			}
		}
	}

	void watcherOnNewFunctionEnable(int index) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onNewFunctionEnable();
	}

	void watcherOnNewFunctionEnable(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onNewFunctionEnable();
				break;
			}
		}
	}

	void watcherOnDeletedFunctionEnable(int index) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onDeleteFunctionEnable();
	}

	void watcherOnDeleteFunctionEnable(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onDeleteFunctionEnable();
				break;
			}
		}
	}

	void watcherOnModifiedFunctionEnable(int index) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->onModifiedFunctionEnable();
	}

	void watcherOnModifiedFunctionEnable(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->onModifiedFunctionEnable();
				break;
			}
		}
	}

	void execute(int index){
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->execute();
	}

	void execute(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->execute();
				break;
			}
		}
	}

	void executeAll()
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			fileWatchers[i]->execute();
		}
	}
	void terminate(int index) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->execute();
	}

	void terminate(string path) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->terminate();
				break;
			}
		}
	}

	void terminateAll()
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			fileWatchers[i]->terminate();
		}
	}

	void textEnable(int index, bool enable = true) {
		if (index < 0 || index >= fileWatchers.size())
		{
			return;
		}
		fileWatchers[index]->textEnable(enable);
	}

	void textEnable(string path, bool enable = true) {
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			if (fileWatchers[i]->getPathName() == path) {
				fileWatchers[i]->textEnable(enable);
				break;
			}
		}
	}

	void textEnableAll(bool enable = true)
	{
		for (int i = 0; i < fileWatchers.size(); i++)
		{
			fileWatchers[i]->textEnable(enable);
		}
	}

  private:
	  vector<FileWatcher *> fileWatchers;
};

}; // namespace FDWatcher
