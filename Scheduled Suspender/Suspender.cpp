#include "stdafx.h"
#include "Suspender.h"
#include <TlHelp32.h>
#include <strsafe.h>
#include "resource.h"

typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

DWORD CSuspender::GetProcessIdByName(LPCWSTR lpwProcessName)
{
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

void CSuspender::MessageDialog(bool toggle, LPCWSTR process_name)
{
	TCHAR message[256];
	TCHAR szMenuName[128];
	StringCchPrintf(szMenuName, STRSAFE_MAX_CCH, L" scheduled suspender | Process Id: %08X (%d)", GetCurrentProcessId(), static_cast<int32_t>(GetCurrentProcessId()));

	if (toggle)
		StringCchPrintf(message, STRSAFE_MAX_CCH, L"Process %d (%s) has been found and suspended.", GetProcessIdByName(process_name), process_name);
	else
		StringCchPrintf(message, STRSAFE_MAX_CCH, L"Process %d (%s) has been resumed.", GetProcessIdByName(process_name), process_name);
	MessageBox(0, message, szMenuName, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_APPLMODAL);
}

void CSuspender::SuspendProcess(DWORD process_id, SUSPEND_METHOD method, bool suspend)
{
	if (!process_id) {
		MessageBoxW(0, L"An error has occured when processing the target process identifier.", L"Error: Scheduled Suspender", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_APPLMODAL);
		return;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (!hProcess) {
		MessageBoxW(0, L"An error has occured when opening the specified local process object with the process identifier of the target process.", L"Error: Scheduled Suspender", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_APPLMODAL);
		return;
	}

	switch (method) {
	case THREADS:
	{
		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(THREADENTRY32);
		Thread32First(hThreadSnapshot, &threadEntry);

		do
		{
			if (threadEntry.th32OwnerProcessID == process_id)
			{
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

				if (suspend) { SuspendThread(hThread); }
				else { ResumeThread(hThread); }

				CloseHandle(hThread);
			}
		} while (Thread32Next(hThreadSnapshot, &threadEntry));

		CloseHandle(hThreadSnapshot);
		break;
	}
	case NT:
	{
		if (suspend) {
			NtSuspendProcess pfnNtSuspendProcess = reinterpret_cast<NtSuspendProcess>(GetProcAddress(GetModuleHandleW(L"ntdll"), "NtSuspendProcess"));
			pfnNtSuspendProcess(hProcess);
		}
		else {
			NtResumeProcess pfnNtResumeProcess = reinterpret_cast<NtResumeProcess>(GetProcAddress(GetModuleHandle(L"ntdll"), "NtResumeProcess"));
			pfnNtResumeProcess(hProcess);
		}
		break;
	}
	case DBG:
	{
		if (suspend) {
			DebugActiveProcess(process_id);
		}
		else {
			DebugActiveProcessStop(process_id);
		}
		break;
	}
	}
	CloseHandle(hProcess);
}

void CSuspender::SuspendProcess(DWORD process_id, SUSPEND_METHOD method)
{
	SuspendProcess(process_id, method, true);
}

void CSuspender::ResumeProcess(DWORD process_id, SUSPEND_METHOD method)
{
	SuspendProcess(process_id, method, false);
}

CSuspender::CSuspender()
{
}


CSuspender::~CSuspender()
{
}



extern bool is_button_checked;
void CSuspender::watch_for_process(tagCSuspender * handle_options)
{
	bool found = false;
	while (is_button_checked) {
		DWORD nProcessId = GetProcessIdByName(handle_options->process_name.c_str());
		if (nProcessId) {
			if (!found) {
				if (handle_options->delay)
					Sleep(handle_options->delay);
				SuspendProcess(nProcessId, handle_options->method);
				MessageDialog(true, handle_options->process_name.c_str());
				found = true;
			}
		}
		else {
			if (found)
			{
				found = false;
			}
		}
	}

	delete handle_options;
}
