#include "stdafx.h"
#include "Pins.h"


CPins::CPins()
	:mCurPin(NULL)
{
}

CPins::CPins(IPin *pin)
	:mCurPin(pin)
{
}

CPins::~CPins()
{
	if (mCurPin)
		mCurPin->Release();
}

bool CPins::setMediaType(GUID majorType, GUID subType, SIZE res, REFERENCE_TIME interval)
{
	HRESULT hr = E_FAIL;
	IEnumMediaTypes *pMediaTypeEnum = NULL;
	IAMStreamConfig *streamCfg;
	AM_MEDIA_TYPE *pmt;
	VIDEO_STREAM_CONFIG_CAPS *pCaps;
	bool bSupportMajor = false;
	bool bSupportMinjor = false;

	if (SUCCEEDED(hr = mCurPin->QueryInterface(IID_IAMStreamConfig, (void**)&streamCfg))){

		int count = 0, size = 0;
		if (SUCCEEDED(hr = streamCfg->GetNumberOfCapabilities(&count, &size))){
			pCaps = new VIDEO_STREAM_CONFIG_CAPS;

			for (int i = 0; i < count; i++){
				if (streamCfg->GetStreamCaps(i, &pmt, (BYTE*)pCaps) == S_OK){
					if (pmt->majortype == majorType){
						bSupportMajor = true;
						if (pmt->subtype == subType){
							bSupportMinjor = true;
							BITMAPINFOHEADER *pVh = dxUtils::getBmpHeader(pmt);
							
						}
					}
					
					dxUtils::FreeMediaType(*pmt);
				}
			}
			delete pCaps;
		}

		streamCfg->Release();
	}
	else if (SUCCEEDED(hr = mCurPin->EnumMediaTypes(&pMediaTypeEnum))){
		while (pMediaTypeEnum->Next(1, &pmt, NULL) == S_OK){
			if (pmt->majortype == majorType){
				bSupportMajor = true;
				if (pmt->subtype == subType){
					bSupportMinjor = true;
				}
			}
			dxUtils::FreeMediaType(*pmt);
		}

		pMediaTypeEnum->Release();
	}

	return bSupportMajor && bSupportMinjor;
}

bool CPins::isRightDir(PIN_DIRECTION dir)
{
	bool bRet = false;
	HRESULT hr = S_OK;
	PIN_DIRECTION pinDir = PINDIR_INPUT;

	hr = mCurPin->QueryDirection(&pinDir);
	if (hr == S_OK){
		if (dir == pinDir){
			bRet = true;
		}
	}

	return bRet;
}

bool CPins::isCanConnect()
{
	IPin *pin = NULL;
	HRESULT hr = S_OK;
	bool bRet = false;

	hr = mCurPin->ConnectedTo(&pin);
	if (hr == S_OK){
		bRet = false;
	}
	else if (hr = VFW_E_CANNOT_CONNECT){
		bRet = true;
	}

	if (pin){
		pin->Release();
	}

	return bRet;
}