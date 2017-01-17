#pragma once

class CSyncRender
{
public:
	CSyncRender();
	~CSyncRender();

	// placeholder
	BOOL PushFrame();
	BOOL GetFrame();

private:
	//render 
	int64_t mLastRenderedFramePts;
	int64_t mRenderInterval;
};

