#include <Windows.h>
#include <string>

#pragma comment(lib, "advapi32.lib")

#pragma once
//class EncryptFile{};
int handleFile(std::string aesKey, std::wstring input, BOOL encrypt);
