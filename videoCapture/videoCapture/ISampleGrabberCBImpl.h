#pragma once
#include <dshow.h>
#include "ISampleGrabber.h"
#include "VideoCapture.h"

class ISampleGrabberCBImpl :
	public ISampleGrabberCB
{
public:
	ISampleGrabberCBImpl(SampleBufferCallback &cb);
	~ISampleGrabberCBImpl();

	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP BufferCB(double SampleTime, BYTE* pBuffer, long BufferSize);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);

private:
	SampleBufferCallback &callback;
};

