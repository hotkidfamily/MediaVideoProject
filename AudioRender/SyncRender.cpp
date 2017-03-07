#include "stdafx.h"
#include "SyncRender.h"


CSyncRender::CSyncRender()
	: mFirstFramePtsBackup(0)
	, mFirstClockBackup(0)
{
}


CSyncRender::~CSyncRender()
{
}

BOOL CSyncRender::Reset()
{
	mRenderBaseClock.ResetBaseTime(0);
	mFirstFramePtsBackup = 0;
	mFirstClockBackup = 0;
	return TRUE;
}

BOOL CSyncRender::PushFrame(VideoSampleBuffer *&frame)
{
	BOOL ret = FALSE;
	FRAMEACTION action = FA_WAIT;

	while (1){ /* time out */
		action = timeToRender(frame->ptsStart);
		switch (action){
		case FA_PUSH:
			ret = TRUE;
		case FA_DROP:
			goto jump;

		case FA_WAIT:
			Sleep(1);
			continue;
		}
jump:
		break;
	}

	mLastPushFramePtsBackup = frame->ptsStart;
	return ret;
}

BOOL CSyncRender::GetFrame()
{
	return TRUE;
}

FRAMEACTION CSyncRender::timeToRender(int64_t ptsIn100ns)
{
	int64_t currenttime = mRenderBaseClock.GetCurrentTimeIn100ns();
	int64_t ptsFlow = ptsIn100ns - mFirstFramePtsBackup;
	int64_t timeFlow = currenttime - mFirstClockBackup;
	int64_t ptsInterval = ptsIn100ns - mLastPushFramePtsBackup;
	int64_t dropThreshold = ptsInterval * 3 / 2;

	logger(Debug, "pts flow %lld, clock %lld, diff %lld\n", ptsFlow, timeFlow, timeFlow - ptsFlow);

	/* 
		step 1. update time base
	*/
	if (mFirstFramePtsBackup == 0){
		mFirstClockBackup = currenttime;
		mFirstFramePtsBackup = ptsIn100ns;
		goto push;
	}

	if (ptsIn100ns <= mFirstFramePtsBackup){
		logger(Debug, "pts < last pts, drop.\n");
		// drop
		goto drop;
	}

	if (ptsFlow <= timeFlow){
		if ((timeFlow - ptsFlow) > dropThreshold){
			logger(Debug, "play slow , drop frame, threshold %lld.\n", dropThreshold);
			goto drop;
		}
		logger(Debug, "pts delta <= render, push\n");
		goto push;
	}else{
		logger(Debug, "pts delta > render. wait\n");
		goto wait;
	}

push:
	return FA_PUSH;

drop:
	return FA_DROP;

wait:
	return FA_WAIT;
}
