#pragma once

#include <dshow.h>
#include <map>
#include <vector>
#include "dxUtils.h"

typedef struct tagDevCaptureOutputFormat{
	int type;
	SIZE outputRes;
	REFERENCE_TIME minFrameInterval;
	REFERENCE_TIME maxFrameInterval;
}DEVCAPOUTPUTFMT;

class CPins
{
public:
	CPins();
	CPins(IPin *pin);
	~CPins();

	bool setMediaType(GUID majorType, GUID subType, SIZE res, REFERENCE_TIME interval);

	bool isRightDir(PIN_DIRECTION dir);
	bool isCanConnect();

private:
	IPin *mCurPin;
};

