#include "stdafx.h"
#include "VideoCallback.h"


CVideoCallback::CVideoCallback()
{
	mBufferManager.Reset(RES1080P, 10);
}


CVideoCallback::~CVideoCallback()
{
}


void CVideoCallback::OnFrame(FRAME_DESC desc)
{
	mBufferManager.FillFrame(desc);
}

void CVideoCallback::OnEvent(EVENT_INDEX index, EVENT_CONTEXT context)
{
	// place hold
}

BOOL CVideoCallback::GetFrame(CSampleBuffer *&pSample)
{
	return mBufferManager.LockFrame(pSample);
}

BOOL CVideoCallback::ReleaseFrame(CSampleBuffer *&pSample)
{
	return mBufferManager.UnlockFrame(pSample);
}
