#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <windowsx.h>
#include <Uxtheme.h>
#include <TlHelp32.h>
#include "resource.h"

using namespace std;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment (lib, "Uxtheme.lib")

typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

bool IsMoving = false;
POINT pCoordinate;

HWND CloseButton;
HWND InfoStatic;
HWND static1;
HWND comboBox1;
HWND static2;
HWND edit1;
HWND checkBox1;
HWND static3;

#define CLOSEBTN 1
#define INFOSTC 2
#define STATIC1 3
#define COMBOBOX1 4
#define STATIC2 5
#define EDIT1 6
#define CHECKBOX1 7
#define STATIC3 8

#define NEW 101
#define EXIT 102
#define ABOUT 103