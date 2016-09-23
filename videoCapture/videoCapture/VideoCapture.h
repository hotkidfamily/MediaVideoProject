// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� VIDEOCAPTURE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// VIDEOCAPTURE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

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


