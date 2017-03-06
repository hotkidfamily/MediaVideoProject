// winVideoCapture.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "assert.h"
#include "winVideoCapture.h"
#include "OpenFile.h"
#include "OpenStream.h"
#include <windowsx.h>

// Global Variables:

THIS_CONTEXT *gContext = nullptr;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(PTHIS_CONTEXT hInstance);
BOOL				InitInstance(PTHIS_CONTEXT, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void AddDevicesToMenu(THIS_CONTEXT *ctx)
{
	HMENU hMainMenu = GetMenu(ctx->hMainWnd);
	HMENU hMenuSub = GetSubMenu(GetMenu(ctx->hMainWnd), 1);
	int iMenuItems = GetMenuItemCount(hMenuSub);
	int index = 0;

	MENUINFO MenuInfo;
	ZeroMemory(&MenuInfo, sizeof(MenuInfo));
	MenuInfo.cbSize = sizeof(MENUINFO);
	MenuInfo.fMask = MIM_STYLE;
	GetMenuInfo(hMainMenu, &MenuInfo);
	MenuInfo.dwStyle |= MNS_NOTIFYBYPOS;
	SetMenuInfo(hMainMenu, &MenuInfo);

	VECT camlist;
	VECT::iterator it;
	ctx->capture->GetDeviceList(camlist);
	ctx->totalCaptureDevices = camlist.size();

	if (iMenuItems > 0){
		for (int i = 0; i < iMenuItems; i++){
			RemoveMenu(hMenuSub, 0, MF_BYPOSITION);
		}
	}

	for (it = camlist.begin(); it != camlist.end(); it++){
		AppendMenu(hMenuSub, MF_STRING, ID_DEVICE_DEVICE3 + index, *it);
		index++;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	gContext = new THIS_CONTEXT;
	assert(gContext);
	gContext->hInstance = hInstance;
	InitializeCriticalSection(&gContext->listLock);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(gContext->hInstance, IDS_APP_TITLE, gContext->szTitle, MAX_LOADSTRING);
	LoadString(gContext->hInstance, IDC_WINVIDEOCAPTURE, gContext->szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(gContext);

	// Perform application initialization:
	if (!InitInstance(gContext, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(gContext->hInstance, MAKEINTRESOURCE(IDC_WINVIDEOCAPTURE));

	gContext->capture = GetVideoCaptureObj();
	assert(gContext->capture);
	AddDevicesToMenu(gContext);

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	StopStream(gContext);
	ReleaseVideoCaptureObj(gContext->capture);
	DeleteCriticalSection(&gContext->listLock);
	delete gContext;
	gContext = nullptr;

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(PTHIS_CONTEXT ctx)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= ctx->hInstance;
	wcex.hIcon			= LoadIcon(ctx->hInstance, MAKEINTRESOURCE(IDI_WINVIDEOCAPTURE));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINVIDEOCAPTURE);
	wcex.lpszClassName	= ctx->szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(THIS_CONTEXT *ctx, int nCmdShow)
{

	ctx->hMainWnd = CreateWindow(ctx->szWindowClass, ctx->szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, ctx->hInstance, nullptr);

	if (!ctx->hMainWnd)
   {
      return FALSE;
   }

	ShowWindow(ctx->hMainWnd, nCmdShow);
	UpdateWindow(ctx->hMainWnd);

   return TRUE;
}

LRESULT CALLBACK StatusDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
	default:
		break;
	}

	return  DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK MediaDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
	default:
		break;
	}

	return  DefWindowProc(hWnd, message, wParam, lParam);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	PAINTSTRUCT ps;
	HDC hdc;
	UINT status = 0;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_MENUCOMMAND:
		{
			HMENU hMenu = (HMENU)lParam;
			int idx = wParam;
			wmId = GetMenuItemID(hMenu, idx);
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(gContext->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case ID_WINDOW_STATIS:
				status = GetMenuState(hMenu, idx, MF_BYPOSITION);
#if 0
				if (status & MF_CHECKED){
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_UNCHECKED);
					ShowWindow(gContext->hDashboardWnd, SW_HIDE);
				}else{
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_CHECKED);
					if (!gContext->hDashboardWnd)
						gContext->hDashboardWnd = CreateDialog(gContext->hInstance, MAKEINTRESOURCE(IDD_DIALOG_STATUS), hWnd, (DLGPROC)StatusDlgProc);
					ShowWindow(gContext->hDashboardWnd, SW_SHOW);
				}
#endif
				break;
			case ID_WINDOW_VIDEO:
				status = GetMenuState(hMenu, idx, MF_BYPOSITION);
#if 0
				if (status & MF_CHECKED){
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_UNCHECKED);
					ShowWindow(gContext->hMediaInfoWnd, SW_HIDE);
				}else{
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_CHECKED);
					if (!gContext->hMediaInfoWnd)
						gContext->hMediaInfoWnd = CreateDialog(gContext->hInstance, MAKEINTRESOURCE(IDD_DIALOG_MEDIA), hWnd, (DLGPROC)MediaDlgProc);
					ShowWindow(gContext->hMediaInfoWnd, SW_SHOW);
				}
#endif
				break;
			case ID_TOOLS_RANDOMRENDER:
				break;
			case ID_FILE_OPEN:
			{
				OpenFileDialog *dlg = new OpenFileDialog(gContext->hMainWnd);
				status = GetMenuState(hMenu, idx, MF_BYPOSITION);
				if (status & MF_CHECKED){
					gContext->captureCfg.filePath.clear();
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_UNCHECKED);
					StopStream(gContext);
				} else{
					if (dlg->ShowDialog()){
						gContext->captureCfg.filePath = dlg->FileName;
						if (SetupStream(gContext, FALSE)){
							SetWindowText(gContext->hMainWnd, gContext->captureCfg.filePath.c_str());
							CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_CHECKED);
						}
					}
				}
			}
				break;
			case ID_FILE_STREAM:
			{
				OpenStreamDialog *dlg = new OpenStreamDialog(gContext->hMainWnd);
				status = GetMenuState(hMenu, idx, MF_BYPOSITION);
				if (status & MF_CHECKED){
					gContext->captureCfg.filePath.clear();
					CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_UNCHECKED);
					StopStream(gContext);
				} else{
					if (dlg->ShowDialog()){
						gContext->captureCfg.filePath = dlg->Path();
						if (SetupStream(gContext, FALSE)){
							SetWindowText(gContext->hMainWnd, gContext->captureCfg.filePath.c_str());
							CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_CHECKED);
						}
					}
				}
			}
				break;
			default:
				if (wmId >= ID_DEVICE_DEVICE3 && wmId < ID_DEVICE_DEVICE3 + gContext->totalCaptureDevices)
				{
					status = GetMenuState(hMenu, idx, MF_BYPOSITION);
					if (status & MF_CHECKED){
						CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_UNCHECKED);
						StopStream(gContext);
					}
					else{
						gContext->captureCfg.index = idx;
						if (SetupStream(gContext, TRUE)){
							SetWindowText(gContext->hMainWnd, gContext->captureCfg.deviceName.c_str());
							CheckMenuItem(hMenu, idx, MF_BYPOSITION | MF_CHECKED);
						}
					}
				}else{
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
