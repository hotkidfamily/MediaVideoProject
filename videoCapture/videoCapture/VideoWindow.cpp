#include "stdafx.h"
#include "VideoWindow.h"

DXVideoWindows::DXVideoWindows(IGraphBuilder *graph)
{
	owerGraph = graph;
}

DXVideoWindows::~DXVideoWindows()
{

}

HRESULT DXVideoWindows::queryVideoWindow()
{
	return owerGraph->QueryInterface(IID_IVideoWindow, (void **)&videoWindow);
}

HRESULT DXVideoWindows::setOwerWindow(HWND hwnd)
{
	owerHWND = hwnd;
	if (IsWindow(hwnd)){
		return videoWindow->put_Owner((OAHWND)hwnd);
	}
	return E_FAIL;
}

HRESULT DXVideoWindows::setOwerWindowStyle()
{
	return videoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
}

HRESULT DXVideoWindows::moveWindow()
{
	RECT rc;
	GetClientRect(owerHWND, &rc);
	return videoWindow->SetWindowPosition(0, 0, rc.right, rc.bottom);
}

HRESULT DXVideoWindows::NotifyOwnerMessage(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam)
{
	return videoWindow->NotifyOwnerMessage((OAHWND)hWnd, msg, wParam, lParam);
}

void DXVideoWindows::cleanUp()
{
	videoWindow->put_Visible(OAFALSE);
	videoWindow->put_Owner(NULL);
}