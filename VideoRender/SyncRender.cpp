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
	mLastRenderedClock = 0;
	mLastRenderedPts = 0;

	return TRUE;
}

BOOL CSyncRender::PushFrame(CSampleBuffer *&frame)
{
	BOOL ret = FALSE;
	int64_t ptss, ptse;
	FRAMEACTION action = FA_WAIT;

	frame->GetPts(ptss, ptse);

	while (1){
		action = timeToRender(ptss);
		switch (action){
		case FA_PUSH:
			ret = TRUE;
			//PUSH
		case FA_DROP:
			goto jump;

		case FA_WAIT:
			Sleep(1);
			continue;
		}
jump:
		break;
	}

	return ret;
}

BOOL CSyncRender::GetFrame()
{
	return TRUE;
}

FRAMEACTION CSyncRender::timeToRender(int64_t ptsIn100ns)
{
	int64_t currenttime = mRenderBaseClock.GetCurrentTimeIn100ns();
	int64_t mRenderInterval = currenttime - mLastRenderedClock;
	int64_t deltapts = ptsIn100ns - mLastRenderedPts;
	int64_t deltaClock = mRenderInterval - deltapts;

	if (mLastRenderedPts == 0){
		goto push;
	}

	if (ptsIn100ns < mLastRenderedPts){
		// drop
		goto drop;
	}

	if (deltapts <= mRenderInterval){
		goto push;
	} else{
		goto wait;
	}

push:
	mLastRenderedClock = mRenderBaseClock.GetCurrentTimeIn100ns();
	mLastRenderedPts = ptsIn100ns;
	return FA_PUSH;

drop:
	return FA_DROP;

wait:
	return FA_WAIT;
}
