#include "stdafx.h"
#include "SyncRender.h"


CSyncRender::CSyncRender()
{
}


CSyncRender::~CSyncRender()
{
}

BOOL CSyncRender::Reset()
{
	mRenderBaseClock.ResetBaseTime(0);

	return TRUE;
}

BOOL CSyncRender::PushFrame(CSampleBuffer *&frame)
{
	BOOL ret = FALSE;
	int64_t ptss, ptse;
	FRAMEACTION action;

	frame->GetPts(ptss, ptse);


	while ((action = timeToRender(ptss)) != FA_PUSH){
		switch (action){
		case FA_PUSH:
			//PUSH
			mLastRenderedPts = ptss;
		case FA_DROP:
			break;
		case FA_WAIT:
			Sleep(1);
			continue;
		}
	}

	return TRUE;
}

BOOL CSyncRender::GetFrame()
{
	return TRUE;
}

FRAMEACTION CSyncRender::timeToRender(int64_t ptsIn100ns)
{
	int64_t mRenderInterval = mRenderBaseClock.GetCurrentTimeIn100ns() - mLastRenderedClock;
	int64_t deltapts = ptsIn100ns - mLastRenderedPts;
	int64_t deltaClock = mRenderInterval - deltapts;

	if (ptsIn100ns < mLastRenderedPts){
		// drop
		return FA_DROP;
	}

	if (deltapts >= mRenderInterval){
		mLastRenderedClock = mRenderBaseClock.GetCurrentTimeIn100ns();
		return FA_PUSH;
	}

	return FA_WAIT;
}
