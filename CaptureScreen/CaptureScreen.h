#pragma once

#include "resource.h"
#include <shellapi.h>

#define MAX_LOADSTRING 100
#define WM_NIF (WM_USER + 1001)
#define SYS_TRAY_ICON 5000

TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;
HWND hWndBtnSysTray, hWndBtnQuit;

static int cxScreen = GetSystemMetrics(SM_CXSCREEN);
static int cyScreen = GetSystemMetrics(SM_CYSCREEN);

static int nWidth = 500;
static int nHeight = 300;

static int cxBegin = (cxScreen - nWidth) / 2;
static int cyBegin = (cyScreen - nHeight) / 2;

#define ID_BTN_QUIT 5000
#define ID_BTN_SYSTRAY 5001

#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH 100

#define ALIGN_LEFT 30
#define ALIGN_BOTTOM 110

static WCHAR* G3 = L"BÀI TẬP G3 - NHÓM 6";
static WCHAR* SV1 = L"1412060 + Huỳnh Minh Chương";
static WCHAR* SV2 = L"1412453 + Phạm Minh Sang";
static WCHAR* GUIDE = L"Nhấn giữ Ctrl và Click vào cửa sổ cần chụp";

LRESULT OnCreate(HWND hWnd)
{

	hWndBtnSysTray = CreateWindow(L"BUTTON", L"System tray",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD |
		BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY,
		ALIGN_LEFT, nHeight - ALIGN_BOTTOM,
		BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)ID_BTN_SYSTRAY, hInst, 0);

	hWndBtnQuit = CreateWindow(L"BUTTON", L"Quit",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD |
		BS_DEFPUSHBUTTON | BS_CENTER | BS_NOTIFY,
		nWidth - BUTTON_WIDTH - ALIGN_LEFT * 2, nHeight - ALIGN_BOTTOM,
		BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)ID_BTN_QUIT, hInst, 0);

	if (hWndBtnSysTray && hWndBtnQuit)
	{
		ShowWindow(hWndBtnSysTray, SW_SHOW);
		ShowWindow(hWndBtnQuit, SW_SHOW);
		return TRUE;
	}

	return FALSE;
}
LRESULT OnSystemTray(HWND hWnd, UINT uID, HICON hIcon, LPCWSTR lpszTip)
{
	BOOL res = FALSE;

	NOTIFYICONDATA nifDt;
	nifDt.cbSize = sizeof(NOTIFYICONDATA);
	nifDt.uID = uID;
	nifDt.hWnd = hWnd;
	nifDt.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	nifDt.uCallbackMessage = WM_NIF;
	nifDt.hIcon = hIcon;

	if (lpszTip)
	{
		lstrcpyn(nifDt.szInfo, lpszTip, sizeof(nifDt.szInfo));
		lstrcpyn(nifDt.szInfoTitle, szTitle, sizeof(nifDt.szInfoTitle));
		lstrcpyn(nifDt.szTip, lpszTip, sizeof(nifDt.szTip));
	}
	else
	{
		nifDt.szTip[0] = '\0';
		nifDt.szInfoTitle[0] = '\0';
		nifDt.szTip[0] = '\0';
	}
	res = Shell_NotifyIcon(NIM_ADD, &nifDt);
	if (hIcon)
		DestroyIcon(hIcon);
	return res;
}

LRESULT OnSYS_BTN_DOWND(HWND hWnd, UINT uID, LPCWSTR lpszTip)
{
	ShowWindow(hWnd, SW_HIDE);
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	return OnSystemTray(hWnd, SYS_TRAY_ICON, hIcon, lpszTip);
}

LRESULT StopSystemTray(HWND hWnd, UINT uID)
{
	BOOL result = FALSE;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hWnd;
	tnid.uID = uID;

	result = Shell_NotifyIcon(NIM_DELETE, &tnid);
	return result;
}
LRESULT OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL res = FALSE;

	if (lParam == WM_LBUTTONDOWN)
	{
		ShowWindow(hWnd, SW_SHOW);
		ShowWindow(hWnd, SW_RESTORE);
		return StopSystemTray(hWnd, SYS_TRAY_ICON);
	}

	return res;
}