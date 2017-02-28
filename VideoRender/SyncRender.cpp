#include "stdafx.h"
#include "SyncRender.h"


CSyncRender::CSyncRender()
{
}


CSyncRender::~CSyncRender()
{
}

BOOL CSyncRender::PushFrame(CSampleBuffer *&frame)
{
	int64_t ptss, ptse;
	frame->GetPts(ptss, ptse);

	if (timeToRender(ptss)){
		// push frame
	} else{
		return FALSE;;
	}

	return TRUE;
}

BOOL CSyncRender::GetFrame()
{
	return TRUE;
}

bool CSyncRender::timeToRender(int64_t ptsIn100ns)
{
	if (ptsIn100ns < mLastRenderedPts){
		// drop
		return true;
	}

	if ((ptsIn100ns - mLastRenderedPts) >= mRenderInterval){
		return true;
	}

	return false;
}
