#include "stdafx.h"
#include "resource.h"
#include "OpenStream.h"

HINSTANCE hInstance;
HWND hDialog;
STRING StreamPath;

LRESULT CALLBACK OpenSteamDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDOK:
		{
			STRING path;
			path.resize(1024);
			int len = GetWindowText(GetDlgItem(hDialog, IDC_EDIT_STREAM_PATH), (LPWSTR)path.c_str(), 1024);
			StreamPath.assign(path.c_str(), len);
		}
		SendMessage(hDialog, WM_CLOSE, 0, 0);
			break;
		case IDCANCEL:
			SendMessage(hDialog, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return  DefWindowProc(hWnd, message, wParam, lParam);
}

OpenStreamDialog::OpenStreamDialog(HWND owner)
{
	this->Owner = owner;
}

OpenStreamDialog::~OpenStreamDialog()
{

}

STRING &OpenStreamDialog::Path()
{
	return StreamPath;
}

int OpenStreamDialog::initDialog()
{
	hDialog = CreateDialog(hInstance,
		MAKEINTRESOURCE(IDD_DIALOG_STREAM),
		Owner,
		(DLGPROC)OpenSteamDialogProc);

	if (!hDialog){
		return -1;
	}

	ShowWindow(hDialog, SW_SHOW);

	MSG  msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (status == -1)
			return -1;
		if (!IsDialogMessage(hDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else{
			if (msg.message == WM_CLOSE){
				break;
			}
		}
	}

	return msg.wParam;
}

bool OpenStreamDialog::ShowDialog()
{
	StreamPath.assign(TEXT(""));
	initDialog();
	return !StreamPath.empty();
}
