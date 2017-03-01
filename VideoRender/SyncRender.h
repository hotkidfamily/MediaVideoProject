#pragma once

typedef enum tagFrameAction{
	FA_DROP = 0,
	FA_WAIT,
	FA_PUSH,
}FRAMEACTION;

class CSyncRender
{
public:
	CSyncRender();
	~CSyncRender();

	// placeholder
	BOOL PushFrame(CSampleBuffer *&frame);
	BOOL GetFrame();

	// new render object, reset base time
	BOOL Reset();

protected:
	FRAMEACTION timeToRender(int64_t ptsIn100ns);

private:
	//render 
	CClock mRenderBaseClock;
	int64_t mLastRenderedPts;
	int64_t mLastRenderedClock;
};

