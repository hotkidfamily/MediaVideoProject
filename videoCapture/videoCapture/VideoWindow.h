#ifndef __DX_VIDEO_WINDOW_H__
#define __DX_VIDEO_WINDOW_H__

#pragma  once

#include <windows.h>
#include <dshow.h>

class DXVideoWindows{

public:
	DXVideoWindows(IGraphBuilder* graph);
	~DXVideoWindows();

	HRESULT queryVideoWindow();
	HRESULT setOwerWindow(HWND hwnd);
	HRESULT setOwerWindowStyle();
	HRESULT moveWindow();
	void cleanUp();
	HRESULT NotifyOwnerMessage(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam);

private:
	HWND owerHWND;
	IVideoWindow *videoWindow;
	IGraphBuilder *owerGraph;
};

#endif //__DX_VIDEO_WINDOW_H__
