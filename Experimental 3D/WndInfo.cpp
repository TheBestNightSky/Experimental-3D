#include "WndInfo.h"

WndInfo::WndInfo(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	width = (rect.right - rect.left);
	height = (rect.bottom - rect.top);
	CRegion.width = (rect.right - rect.left) - 16;
	CRegion.height = (rect.bottom - rect.top) - 39;
}

WndInfo::WndInfo() {
	width = 0;
	height = 0;
	CRegion.width = 0;
	CRegion.height = 0;
}