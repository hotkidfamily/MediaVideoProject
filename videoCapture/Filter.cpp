#include "stdafx.h"
#include "Filter.h"


CFilter::CFilter()
	:mCurWorkFilter(NULL)
{
}

CFilter::CFilter(IBaseFilter *filter)
	: mCurWorkFilter(filter)
{
}

CFilter::~CFilter()
{
	if (mCurWorkFilter)
		mCurWorkFilter->Release();
}

CPins * CFilter::getPinByResOrFps(SIZE outputSize, int fps, bool fpsFirst)
{
	HRESULT hr = S_OK;
	IEnumPins *pEnumPins = NULL;
	IPin *pin;
	hr = mCurWorkFilter->EnumPins(&pEnumPins);
	if (hr != S_OK){
		//
	}
	else{
		while (pEnumPins->Next(1, &pin, NULL) == S_OK){
			CPins *curPin = new CPins(pin);
			
			if (curPin->isRightDir(PINDIR_OUTPUT) && curPin->isCanConnect()){
				return curPin;
			}
			delete curPin;
		}
	}

	if (pEnumPins){
		pEnumPins->Release();
	}
	return NULL;
}

CPins * CFilter::getPinByDir(PIN_DIRECTION reqDir)
{
	HRESULT hr = S_OK;
	IEnumPins *pEnumPins = NULL;
	IPin *pin;
	hr = mCurWorkFilter->EnumPins(&pEnumPins);
	if (hr != S_OK){
		//
	}
	else{
		while (pEnumPins->Next(1, &pin, NULL) == S_OK){
			CPins *curPin = new CPins(pin);
			if (curPin->isRightDir(reqDir) && curPin->isCanConnect()){
				return curPin;
			}
			delete curPin;
		}
	}

	if (pEnumPins){
		pEnumPins->Release();
	}
	return NULL;
}

void CFilter::releasePin(CPins *pin)
{
	if (pin)
	delete pin;
}