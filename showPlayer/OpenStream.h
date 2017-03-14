#pragma once
#include <windows.h>


class OpenStreamDialog
{
public:
	OpenStreamDialog(HWND owner);
	~OpenStreamDialog();

	HWND Owner;
	TCHAR*Title;
	STRING &Path();

	bool ShowDialog();

protected:
	int initDialog();
	LRESULT DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

};


