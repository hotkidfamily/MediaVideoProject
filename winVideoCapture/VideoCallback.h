#pragma once

#include "ivideocapture.h"

class CVideoCallback :
	public VideoCaptureCallback
{
public:
	CVideoCallback();
	~CVideoCallback();

	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) ;

private:
};

