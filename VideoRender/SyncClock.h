#pragma once

class CSyncClock
{
public:
	CSyncClock();
	~CSyncClock();

private:
	//render 
	int64_t mLastRenderedFramePts;
	int64_t mRenderInterval;
};

