#pragma once

#include <dshow.h>
#include "ISampleGrabber.h"
#include "logger.h"
#include "ISampleGrabberCBImpl.h"

class dxVideoCapture : public ISampleGrabberCBImpl
{
public:
	dxVideoCapture(logger &log);
	~dxVideoCapture();

	HRESULT initGraph();
	HRESULT buildGraph(IBaseFilter* captureFilter);
	HRESULT start();
	HRESULT stop();

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

private:
	IGraphBuilder *m_filterGraph;
	IMediaControl *m_mediaControl;
	IMediaEvent *m_MediaEvent;
	ICaptureGraphBuilder2 *m_captureGraphBuilder;
	
	IBaseFilter *m_nullRenderFilter;

	IBaseFilter *m_sampleGrabberFilter;
	ISampleGrabber *m_sampleGrabber;

	logger &log;
};

