#pragma once

class CSyncRender
{
public:
	CSyncRender();
	~CSyncRender();

	// placeholder
	BOOL PushFrame(CSampleBuffer *&frame);
	BOOL GetFrame();
protected:
	bool timeToRender(int64_t ptsIn100ns);

private:
	//render 
	CClock mRenderBaseClock;
	int64_t mLastRenderedPts;
	int64_t mRenderInterval;
};

