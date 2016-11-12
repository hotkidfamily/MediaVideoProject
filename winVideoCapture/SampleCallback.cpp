#include "stdafx.h"
#include "SampleCallback.h"


CSampleCallback::CSampleCallback()
{
	mBufferManager.Reset(RES1080P, 10);
}


CSampleCallback::~CSampleCallback()
{
}


void CSampleCallback::OnFrame(FRAME_DESC desc)
{
	mBufferManager.FillOneFrame(desc.dataPtr, desc.dataSize, desc.ptsStart, desc.pixelFormatInFourCC);
}

void CSampleCallback::OnEvent(EVENT_INDEX index, EVENT_CONTEXT context)
{
	// place hold
}

BOOL CSampleCallback::GetFrame(CSampleBuffer *pSample)
{
	mBufferManager.LockFrame(pSample);

	return TRUE;
}

BOOL CSampleCallback::ReleaseFrame(CSampleBuffer * pSample)
{
	mBufferManager.UnlockFrame(pSample);
	return TRUE;
}
