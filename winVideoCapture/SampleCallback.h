#pragma once

#include "SampleBufferManager.h"

class CSampleCallback :
	public VideoCaptureCallback
{
public:
	CSampleCallback();
	~CSampleCallback();

	virtual void OnFrame(FRAME_DESC);
	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) ;

	// thread just get frame, when per 10ms??
	BOOL GetFrame(CSampleBuffer *);
	BOOL ReleaseFrame(CSampleBuffer *);

private:
	CSampleBufferManager mBufferManager;
};

