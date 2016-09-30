#pragma once

#include <dshow.h>
#include "ISampleGrabber.h"
#include "logger.h"
#include "ISampleGrabberCBImpl.h"
#include "SlidingWindowCalc.h"

#define WM_GRAPHNOTIFY WM_APP+1

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
	HRESULT selectMostSuiltableOutputFormat(IBaseFilter* captureFilter);
	TCHAR *guid2str(GUID id);
	TCHAR *fourCCStr(DWORD fourCC);
	void FreeMediaType(AM_MEDIA_TYPE& mt);
	HRESULT removeFilters();
	HRESULT openSampleGrabber();
	
	HRESULT SetupVideoWindow(void);
	
private:
	IGraphBuilder *m_filterGraph;
	IMediaControl *m_mediaControl;
	IMediaEventEx *m_MediaEvent;
	ICaptureGraphBuilder2 *m_captureGraphBuilder;
	
	IBaseFilter *m_nullRenderFilter;

	IBaseFilter *m_sampleGrabberFilter;
	ISampleGrabber *m_sampleGrabber;

	IVideoWindow  * m_pVM;
	HWND m_hWnd;

	logger &log;

	CSlidingWindowCalc statics;
};

