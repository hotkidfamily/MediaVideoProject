// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 VIDEOCAPTURE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// VIDEOCAPTURE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#pragma once 

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif

#include <assert.h>

#include <dshow.h>
#include <string>

class SampleBufferCallback{
public:
	virtual STDMETHODIMP OnSampleCallback(double SampleTime, IMediaSample *pSample) = 0;
};


HRESULT VIDEOCAPTURE_API InitVideoCapture();
HRESULT VIDEOCAPTURE_API DeinitVideoCapture();
HRESULT VIDEOCAPTURE_API SetSampleCallback(SampleBufferCallback &cb);
HRESULT VIDEOCAPTURE_API GrabVideoBitmap(PCWSTR pszVideoFile, PCWSTR pszBitmapFile);
HRESULT VIDEOCAPTURE_API MoveVideoWindowPos(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam);
HRESULT WriteBitmap(PCWSTR pszFileName, BITMAPINFOHEADER *pBMI, size_t cbBMI, BYTE *pData, size_t cbData);


