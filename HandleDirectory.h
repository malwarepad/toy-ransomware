#include <Windows.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

#include "HandleFile.h"

int handleDirectory(std::string privateKey, std::wstring base, BOOL encrypt);