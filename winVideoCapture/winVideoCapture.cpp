// winVideoCapture.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "assert.h"
#include "winVideoCapture.h"
#include <windowsx.h>
#include "VideoCallback.h"
#include "dshowutil.h"


#define MAX_LOADSTRING 100

// Global Variables:

typedef struct tagProgramContext{
	HINSTANCE hInst;
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	HWND mainWnd;
	IVideoCapture *pVideoCapture;
	CVideoCallback *callBack;
	BOOL bRuning;
	HANDLE workThread;
	DWORD dwThreadId;
	tagProgramContext(){
		ZeroMemory(this, sizeof(struct tagProgramContext));
	}
}THIS_CONTEXT, *PTHIS_CONTEXT;

THIS_CONTEXT *gContext = NULL;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(PTHIS_CONTEXT hInstance);
BOOL				InitInstance(PTHIS_CONTEXT, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void AddDevicesToMenu(THIS_CONTEXT *ctx)
{
	HMENU hMenuSub;
	hMenuSub = GetSubMenu(GetMenu(ctx->mainWnd), 1);
	int iMenuItems = GetMenuItemCount(hMenuSub);

	VECT camlist;
	ctx->pVideoCapture->GetDeviceList(camlist);
	VECT::iterator it;
	int index = 0;

	if (iMenuItems > 0){
		for (int i = 0; i < iMenuItems; i++)
		{
			RemoveMenu(hMenuSub, 0, MF_BYPOSITION);
		}
	}

	for (it = camlist.begin(); it != camlist.end(); it++){
		AppendMenu(hMenuSub, MF_STRING, ID_DEVICE_DEVICE3+index, *it);
		index++;
	}

	CheckMenuItem(hMenuSub, ID_DEVICE_DEVICE3+0, MF_CHECKED);
	EnableMenuItem(hMenuSub, ID_DEVICE_DEVICE3+0, MF_ENABLED);
}

BOOL StartWork(THIS_CONTEXT *ctx)
{
	BOOL bRet = FALSE;

	if (!ctx->bRuning){
		ctx->callBack = new CVideoCallback;
		assert(ctx->callBack);
		ctx->pVideoCapture->RegisterCallback(ctx->callBack);

		OPEN_DEVICE_PARAM devices;
		devices.parentWindow = ctx->mainWnd;
		devices.index = 0;
		devices.avgFrameIntervalInNs = FramesPerSecToRefTime(30);
		devices.width = 1920;
		devices.height = 1080;
		bRet = ctx->pVideoCapture->StartCaptureWithParam(devices);
		ctx->bRuning = TRUE;
	}
	else{
		bRet = TRUE;
	}

	return bRet;
}

BOOL StopWork(THIS_CONTEXT *ctx)
{
	assert(ctx);
	ctx->pVideoCapture->StopCapture();
	ctx->pVideoCapture->UnRegisterCallback();

	SAFE_DELETE(ctx->callBack);

	ReleaseVideoCaptureObj(ctx->pVideoCapture);
	return TRUE;
}

BOOL CreateWorkThread(THIS_CONTEXT *ctx)
{
	return TRUE;
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
	gContext->hInst = hInstance;

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(gContext->hInst, IDS_APP_TITLE, gContext->szTitle, MAX_LOADSTRING);
	LoadString(gContext->hInst, IDC_WINVIDEOCAPTURE, gContext->szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(gContext);

	// Perform application initialization:
	if (!InitInstance(gContext, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(gContext->hInst, MAKEINTRESOURCE(IDC_WINVIDEOCAPTURE));

	gContext->pVideoCapture = GetVideoCaptureObj();
	assert(gContext->pVideoCapture);
	AddDevicesToMenu(gContext);
	CreateWorkThread(gContext);
	StartWork(gContext);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	StopWork(gContext);
	gContext = NULL;
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
	wcex.hInstance		= ctx->hInst;
	wcex.hIcon			= LoadIcon(ctx->hInst, MAKEINTRESOURCE(IDI_WINVIDEOCAPTURE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
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

	ctx->mainWnd = CreateWindow(ctx->szWindowClass, ctx->szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, ctx->hInst, NULL);

	if (!ctx->mainWnd)
   {
      return FALSE;
   }

	ShowWindow(ctx->mainWnd, nCmdShow);
	UpdateWindow(ctx->mainWnd);

   return TRUE;
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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(gContext->hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_DEVICE_DEVICE3+0:
			
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		if (gContext->pVideoCapture)
			gContext->pVideoCapture->Repaint(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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
