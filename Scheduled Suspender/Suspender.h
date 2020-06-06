#pragma once
#include <string>

enum SUSPEND_METHOD {
	THREADS = 1,
	NT = 2,
	DBG = 3
};

struct tagCSuspender {
	SUSPEND_METHOD method;
	std::wstring process_name;
	DWORD delay;
};

class CSuspender
{
public:
	CSuspender();
	~CSuspender();

	static void watch_for_process(tagCSuspender *handle_options);
	static DWORD GetProcessIdByName(LPCWSTR lpwProcessName);
	static void MessageDialog(bool toggle, LPCWSTR process_name);
	static void SuspendProcess(DWORD process_id, SUSPEND_METHOD method, bool suspend);
	static void SuspendProcess(DWORD process_id, SUSPEND_METHOD method);
	static void ResumeProcess(DWORD process_id, SUSPEND_METHOD method);
};

