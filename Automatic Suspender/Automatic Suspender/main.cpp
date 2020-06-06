#include "main.h"

DWORD GetProcessIdByName(LPWSTR lpwProcessName) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE || hProcessSnap == NULL) {
		return 0;
	}

	if (Process32First(hProcessSnap, &pe32)) {
		while (Process32Next(hProcessSnap, &pe32)) {
			if (lstrcmpiW(lpwProcessName, pe32.szExeFile) == 0) {
				if (hProcessSnap) {
					CloseHandle(hProcessSnap);
				}
				return pe32.th32ProcessID;
			}
		}
	}

	if (hProcessSnap) {
		CloseHandle(hProcessSnap);
	}

	return 0;
}

BOOL IsProcessRunningW(LPWSTR lpwProcessName) {
	if (GetProcessIdByName(lpwProcessName)) {
		return TRUE;
	}
	return FALSE;
}

void SuspendProcess(DWORD dwProcess, BOOL nSuspend) {
	if (!dwProcess) {
		MessageBoxW(0, L"An error has occured when processing the target process identifier.", L"Error: Automatic Suspender", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_APPLMODAL);
		return;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
	if (!hProcess) {
		MessageBoxW(0, L"An error has occured when opening the specified local process object with the process identifier of the target process.", L"Error: Automatic Suspender", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_APPLMODAL);
		return;
	}

	int t = ComboBox_GetCurSel(comboBox1);
	switch (t) {
	case 0:
	{
		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(THREADENTRY32);

		Thread32First(hThreadSnapshot, &threadEntry);

		do
		{
			if (threadEntry.th32OwnerProcessID == dwProcess)
			{
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

				if (nSuspend) { SuspendThread(hThread); }
				else { ResumeThread(hThread); }
				
				CloseHandle(hThread);
			}
		} while (Thread32Next(hThreadSnapshot, &threadEntry));

		CloseHandle(hThreadSnapshot);
		break;
	}
	case 1:
	{
		if (nSuspend) {
			NtSuspendProcess pfnNtSuspendProcess = reinterpret_cast<NtSuspendProcess>(GetProcAddress(GetModuleHandleW(L"ntdll"), "NtSuspendProcess"));
			pfnNtSuspendProcess(hProcess);
		}
		else {
			NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(GetModuleHandle(L"ntdll"), "NtResumeProcess");
			pfnNtResumeProcess(hProcess);
		}
		break;
	}
	case 2:
	{
		if (nSuspend) {
			DebugActiveProcess(dwProcess);
		}
		else {
			DebugActiveProcessStop(dwProcess);
		}
		break;
	}
	}
	CloseHandle(hProcess);
}

void SuspendProcess(DWORD dwProcess) {
	SuspendProcess(dwProcess, TRUE);
}

void ResumeProcess(DWORD dwProcess) {
	SuspendProcess(dwProcess, FALSE);
}

void SuccessMessage(BOOL Frozen, WCHAR ProcessChar[]) {
	WCHAR message[2048];
	WCHAR message_title[128];
	GetWindowText(InfoStatic, message_title, GetWindowTextLength(InfoStatic) + 1);
	if (Frozen)
		wsprintf(message, L"Process %d (%s) has been successfully frozen. Uncheck the checkbox once you wish to unfreeze it.", GetProcessIdByName(ProcessChar), ProcessChar);
	else
		wsprintf(message, L"Process %d (%s) has been unfrozen.", GetProcessIdByName(ProcessChar), ProcessChar);
	MessageBox(0, message, message_title, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL);
}

BOOL n_watcher_thread = FALSE;
void watcher_thread() {
	WCHAR ProcessChar[512];
	GetWindowText(edit1, ProcessChar, GetWindowTextLength(edit1) + 1);
	bool b = false;

	while (n_watcher_thread) {
		DWORD nProcessId = GetProcessIdByName(ProcessChar);
		if (nProcessId) {
			if (!b) {
				SuspendProcess(nProcessId);
				SuccessMessage(TRUE, ProcessChar);
				b = true;
			}
		}
		else {
			if (b)
			{
				b = false;
			}
		}
	}
}

BOOL IsRunningAsAdministrator() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

void ui_suspend_resume(HWND hwnd) {
	if (IsDlgButtonChecked(hwnd, CHECKBOX1))
	{
		CheckDlgButton(hwnd, CHECKBOX1, BST_UNCHECKED);
		n_watcher_thread = FALSE;
		ComboBox_Enable(comboBox1, TRUE);
		EnableWindow(edit1, TRUE);
		wchar_t ProcessChar[2048];
		GetWindowText(edit1, ProcessChar, GetWindowTextLength(edit1) + 1);
		ResumeProcess(GetProcessIdByName(ProcessChar));
		SuccessMessage(false, ProcessChar);
	}
	else
	{
		if (ComboBox_GetCurSel(comboBox1) == 2) {
			if (!(MessageBox(hwnd, L"Warning: After resuming the process with Debugger Suspend Process; all the threads will be dead; so it would appear as if it is still suspended. Do you wish you continue?", L"Automatic Suspender Warning", MB_YESNO | MB_SETFOREGROUND | MB_ICONWARNING | MB_APPLMODAL) == IDYES)) {
				return;
			}
		}
		CheckDlgButton(hwnd, CHECKBOX1, BST_CHECKED);
		n_watcher_thread = TRUE;
		EnableWindow(comboBox1, FALSE);
		EnableWindow(edit1, FALSE);
		CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&watcher_thread), NULL, 0, NULL);
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		WCHAR window_title[2048];
		wsprintfW(window_title, L"Automatic Suspender <ID: %d>", GetCurrentProcessId());

		CloseButton = CreateWindowEx(0L, L"STATIC", L"x", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY, 380, 0, 20, 18, hwnd, reinterpret_cast<HMENU>(CLOSEBTN), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		InfoStatic = CreateWindowEx(0L, L"STATIC", window_title, WS_VISIBLE | WS_CHILD | SS_CENTER, 0, 0, 365, 20, hwnd, reinterpret_cast<HMENU>(INFOSTC), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		static1 = CreateWindowEx(0L, L"STATIC", L"Technique: ", WS_VISIBLE | WS_CHILD, 5, 28, 80, 20, hwnd, reinterpret_cast<HMENU>(STATIC1), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		comboBox1 = CreateWindowEx(0L, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL, 85, 25, 310, 20, hwnd, reinterpret_cast<HMENU>(COMBOBOX1), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		static2 = CreateWindowEx(0L, L"STATIC", L"Target Process: ", WS_VISIBLE | WS_CHILD, 5, 55, 120, 20, hwnd, reinterpret_cast<HMENU>(STATIC1), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		edit1 = CreateWindowEx(0L, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_CENTER | ES_AUTOHSCROLL | WS_BORDER, 125, 55, 270, 20, hwnd, reinterpret_cast<HMENU>(EDIT1), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		static3 = CreateWindowEx(0L, L"STATIC", L"Automatic Suspend", WS_VISIBLE | WS_CHILD | SS_NOTIFY, 275, 80, 180, 20, hwnd, reinterpret_cast<HMENU>(STATIC3), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);
		checkBox1 = CreateWindowEx(0L, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 255, 78, 20, 20, hwnd, reinterpret_cast<HMENU>(CHECKBOX1), reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, NULL);

		ComboBox_AddItemData(comboBox1, L"Suspend All Threads");
		ComboBox_AddItemData(comboBox1, L"Suspend Process");
		ComboBox_AddItemData(comboBox1, L"Debugger Suspend Process");
		ComboBox_SetCurSel(comboBox1, 0);

		HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Consolas");
		SendMessage(CloseButton, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Consolas")), static_cast<LPARAM>(false));
		SendMessage(InfoStatic, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(static1, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(comboBox1, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(static2, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(edit1, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(static3, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), static_cast<LPARAM>(false));
		SendMessage(checkBox1, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI")), static_cast<LPARAM>(false));
		break;
	}
	case WM_COMMAND: {
		switch (wParam) {
		case NEW:
		{
			WCHAR wc[2048];
			GetModuleFileNameW(NULL, wc, sizeof(wc));
			ShellExecuteW(NULL, L"OPEN", wc, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		case EXIT: {
			PostQuitMessage(0);
			break;
		}
		case ABOUT: {
			MessageBox(hwnd, L"Automatic Suspender		1.0.2\n\nCreated by Xenon[icalz]", L"About Automatic Suspender", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL | MB_SETFOREGROUND);
			break;
		}
		case CHECKBOX1: {
			ui_suspend_resume(hwnd);
			break;
		}
		}

		if (HIWORD(wParam) == STN_CLICKED) {
			if (lParam == reinterpret_cast<LPARAM>(CloseButton))
			{
				Sleep(100);
				PostQuitMessage(0);
			}
			else if (lParam == reinterpret_cast<LPARAM>(static3))
			{
				ui_suspend_resume(hwnd);
			}
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		if (lParam == reinterpret_cast<LPARAM>(CloseButton) ||
			lParam == reinterpret_cast<LPARAM>(InfoStatic))
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(255, 255, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_WINDOWTEXT);
		}
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)CreateSolidBrush(RGB(24, 24, 24));
	}
	case WM_CTLCOLOREDIT: {
		if (lParam == reinterpret_cast<LPARAM>(edit1)) {
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(255, 255, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_WINDOWTEXT);
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		POINT pCursor;
		GetCursorPos(&pCursor);
		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NEW, L"new");
		InsertMenu(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, EXIT, L"exit");
		InsertMenu(hPopupMenu, 2, MF_BYPOSITION | MF_SEPARATOR, 0, L"");
		InsertMenu(hPopupMenu, 3, MF_BYPOSITION | MF_STRING, ABOUT, L"about automatic suspender");
		TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN, pCursor.x, pCursor.y, 0, hwnd, NULL);
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam == MK_LBUTTON)
		{
			if (IsMoving)
			{
				POINT p;
				GetCursorPos(&p);
				SetWindowPos(hwnd, NULL, p.x - pCoordinate.x, p.y - pCoordinate.y, 0, 0, SWP_NOSIZE);
			}
		}
		else
		{
			IsMoving = false;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		IsMoving = true;
		pCoordinate.x = LOWORD(lParam);
		pCoordinate.y = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		IsMoving = false;
		break;
	}


	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int32_t CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow) {
	if (IsRunningAsAdministrator() == FALSE) {
		MessageBox(NULL, L"Please execute Automatic Suspender with full administrative privileges. The program will now exit.", L"Error: Automatic Suspender", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return 0;
	}

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW | CS_NOCLOSE;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(24, 24, 24));
	wc.lpszMenuName = L"Automatic Suspender";
	wc.lpszClassName = L"Automatic Suspender";
	wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	if (!RegisterClassEx(&wc)) {
		return 1;
	}

	HWND hWnd = CreateWindow(wc.lpszClassName, wc.lpszMenuName, WS_POPUP, static_cast<int32_t>(GetSystemMetrics(SM_CXSCREEN) / 4), static_cast<int32_t>(GetSystemMetrics(SM_CYSCREEN) / 4), 400, 100, NULL, NULL, wc.hInstance, NULL);

	if (!hWnd) {
		return 1;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int32_t>(msg.wParam);
}