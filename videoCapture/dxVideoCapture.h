#pragma once

#include <dshow.h>
#include <vector>
#include <windef.h>
#include "ISampleGrabber.h"
#include "logger.h"
#include "ISampleGrabberCBImpl.h"
#include "SlidingWindowCalc.h"
#include "dxUtils.h"

#define WM_GRAPHNOTIFY WM_APP+1

typedef struct tagOutputInfo
{
	SIZE OutputSize;
	LONGLONG MinFrameInterval;
	LONGLONG MaxFrameInterval;
}OutputFormat;

class dxVideoCapture : public ISampleGrabberCBImpl
{
public:
	dxVideoCapture(logger &log);
	~dxVideoCapture();

	HRESULT initGraph(HWND hWnd);
	HRESULT buildGraph(IBaseFilter* captureFilter);
	HRESULT start();
	HRESULT stop();

	HRESULT HandleGraphEvent(void);
	void ResizeVideoWindow(void);

	HRESULT showFilterPropertyPages(IBaseFilter *filter);

	bool isRuning();

	STDMETHODIMP ISampleGrabberCBImpl::SampleCB(double SampleTime, IMediaSample *pSample);

protected:
	HRESULT removeFilters();
	HRESULT selectSuitablePin(IBaseFilter *filter);
	HRESULT configSampleGrabber();
	HRESULT setCaptureFormat();
	HRESULT getCaptureFormat();
	HRESULT SetupVideoWindow(void);
	
private:
	IGraphBuilder *mFilterGraph;
	IMediaControl *mFilterGraphMediaControl;
	IMediaEventEx *mFilterGraphMediaEvent;
	ICaptureGraphBuilder2 *mFilterGraphBuilder;
	
	IBaseFilter *mNullRenderFilter;

	IBaseFilter *mSampleGrabberFilter;
	ISampleGrabber *mSampleGrabber;

	IVideoWindow  * mFilterGraphVideoWindow;
	HWND m_hWnd;

	logger &log;

	CSlidingWindowCalc statics;

	std::vector<OutputFormat> outputRes;
};

