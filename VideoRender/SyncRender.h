#pragma once

class CSyncRender
{
public:
	CSyncRender();
	~CSyncRender();

private:
	//render 
	int64_t mLastRenderedFramePts;
	int64_t mRenderInterval;
};

