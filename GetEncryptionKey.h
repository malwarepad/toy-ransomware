#include <Windows.h>
#include <string>
#include <Winhttp.h>
#include <map>

#include "ParseJSON.h"

#pragma comment(lib, "winhttp.lib")

int getEncryptionKey(std::string* out);
