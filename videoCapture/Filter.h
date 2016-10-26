#pragma once

#include <dshow.h>
#include "Pins.h"

class CFilter
{
public:
	CFilter();
	CFilter(IBaseFilter *filter);
	~CFilter();

	inline IBaseFilter *operator=(IBaseFilter *filter){ return mCurWorkFilter; }

	CPins * getPinByDir(PIN_DIRECTION reqDir);
	CPins * getPinByResOrFps(SIZE outputSize, int fps, bool fpsFirst);
	void releasePin (CPins *pin);

private:
	IBaseFilter *mCurWorkFilter;
};

