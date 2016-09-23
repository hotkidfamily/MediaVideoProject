#include "stdafx.h"
#include "ISampleGrabberCBImpl.h"


ISampleGrabberCBImpl::ISampleGrabberCBImpl(SampleBufferCallback &cb)
	: callback(cb)
{
}


ISampleGrabberCBImpl::~ISampleGrabberCBImpl()
{
}


STDMETHODIMP ISampleGrabberCBImpl::QueryInterface(REFIID iid, void** ppv)
{
	if (NULL == ppv)
		return E_POINTER;

	*ppv = NULL;
	if (IID_IUnknown == iid)
	{
		*ppv = (IUnknown*)this;
		AddRef();
		return S_OK;
	}
	else if (IID_ISampleGrabberCB == iid)
	{
		*ppv = (ISampleGrabberCB*)this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP ISampleGrabberCBImpl::BufferCB(double SampleTime, BYTE* pBuffer, long BufferSize)
{
	return S_OK;
}

STDMETHODIMP ISampleGrabberCBImpl::SampleCB(double SampleTime, IMediaSample *pSample)
{
	return callback.OnSampleCallback(SampleTime, pSample);
}
