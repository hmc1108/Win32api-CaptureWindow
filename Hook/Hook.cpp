// Hook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Hook.h"
#include <ole2.h>
#include <olectl.h>
int widthRatio;
int heightRatio;
bool saveBitmap(LPCWSTR filename, HBITMAP bmp, HPALETTE pal);

bool screenCapturePart(int x, int y, int w, int h, LPCWSTR fname){
	HDC hdcSource = GetDC(NULL);
	HDC hdcMemory = CreateCompatibleDC(hdcSource);

	int capX = GetDeviceCaps(hdcSource, HORZRES);
	int capY = GetDeviceCaps(hdcSource, VERTRES);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

	BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

	DeleteDC(hdcSource);
	DeleteDC(hdcMemory);

	HPALETTE hpal = NULL;
	if (saveBitmap(fname, hBitmap, hpal)) return true;
	return false;
}

bool saveBitmap(LPCWSTR filename, HBITMAP bmp, HPALETTE pal)
{
	bool result = false;
	PICTDESC pd;

	pd.cbSizeofstruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_BITMAP;
	pd.bmp.hbitmap = bmp;
	pd.bmp.hpal = pal;

	LPPICTURE picture;
	HRESULT res = OleCreatePictureIndirect(&pd, IID_IPicture, false,
		reinterpret_cast<void**>(&picture));

	if (!SUCCEEDED(res))
		return false;

	LPSTREAM stream;
	res = CreateStreamOnHGlobal(0, true, &stream);

	if (!SUCCEEDED(res))
	{
		picture->Release();
		return false;
	}

	LONG bytes_streamed;
	res = picture->SaveAsFile(stream, true, &bytes_streamed);

	HANDLE file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (!SUCCEEDED(res) || !file)
	{
		stream->Release();
		picture->Release();
		return false;
	}

	HGLOBAL mem = 0;
	GetHGlobalFromStream(stream, &mem);
	LPVOID data = GlobalLock(mem);

	DWORD bytes_written;

	result = !!WriteFile(file, data, bytes_streamed, &bytes_written, 0);
	result &= (bytes_written == static_cast<DWORD>(bytes_streamed));

	GlobalUnlock(mem);
	CloseHandle(file);

	stream->Release();
	picture->Release();

	return result;
}

LRESULT CALLBACK hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) // không xử lý message 
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	if (GetAsyncKeyState(VK_SNAPSHOT) && wParam == WM_LBUTTONDOWN)
	{
		MOUSEHOOKSTRUCT *mHookData = (MOUSEHOOKSTRUCT *)lParam;
		HWND hWnd = WindowFromPoint(mHookData->pt);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		if (hWnd == NULL)
			MessageBox(hWnd, L"Get HWND failed", L"Result", MB_OK);
		screenCapturePart(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), L"noname.bmp");
		WinExec("mspaint noname.bmp", SW_SHOWNORMAL);
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

extern "C" HOOK_API void _doInstallHook(HWND hWnd)
{
	hWndAll = hWnd;
	if (hHook != NULL) return;

	hHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)hookProc, hinstLib, 0);
}

extern "C" HOOK_API void _doRemoveHook(HWND hWnd)
{
	if (hHook == NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}