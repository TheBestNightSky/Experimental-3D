#pragma once
#include "SkyeWin.h"

class WndInfo {
public:
	class CRegionInfo {
	public:
		LONG width;
		LONG height;
	};
	LONG width;
	LONG height;
	CRegionInfo CRegion;

	WndInfo(HWND hWnd);
	WndInfo();
};