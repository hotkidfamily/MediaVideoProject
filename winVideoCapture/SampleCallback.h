#pragma once

class CSampleCallback :
	public VideoCaptureCallback
{
public:
	CSampleCallback();
	~CSampleCallback();

	virtual void OnFrame(FRAME_DESC);
	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) ;

	bool GetOneFrame();
};

