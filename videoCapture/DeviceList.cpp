#include "stdafx.h"
#include "DeviceList.h"

DeviceList::DeviceList()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

DeviceList::~DeviceList()
{
	CoUninitialize();
}

void DeviceList::addOutputFormat(AM_MEDIA_TYPE *pmt, VIDEO_STREAM_CONFIG_CAPS *pCaps)
{
	DEVCAPOUTPUTFMT output;
	BITMAPINFOHEADER *pvh = dxUtils::getBmpHeader(pmt);

	output.type = dxUtils::getVideoType(pmt);

	output.outputRes.cx = pvh->biWidth;
	output.outputRes.cy = abs(pvh->biHeight);
	output.minFrameInterval = pCaps->MinFrameInterval;
	output.maxFrameInterval = pCaps->MaxFrameInterval;

	devOutputFormats.push_back(output);
}

HRESULT DeviceList::enumPins(IBaseFilter *captureFilter)
{
	HRESULT hr = E_FAIL;
	ULONG ulCnt = 0;

	IEnumPins *pEnum = NULL;
	IPin *pPins = NULL;

	int pinCnt = 0;

	hr = captureFilter->EnumPins(&pEnum);
	if (FAILED(hr)){
		goto done;
	}

	while (pEnum->Next(1, &pPins, &ulCnt) == S_OK){
		PIN_INFO pinInfo = { 0 };

		if (pPins->QueryPinInfo(&pinInfo) == S_OK){
			if (pinInfo.dir != PINDIR_OUTPUT){
				pPins->Release();
				continue;
			}

			IEnumMediaTypes *pMediaTypeEnum;
			pPins->EnumMediaTypes(&pMediaTypeEnum);

			AM_MEDIA_TYPE *pType;
			while(pMediaTypeEnum->Next(1, &pType, NULL) == S_OK){
				//log.
			}

			IAMStreamConfig *streamCfg = NULL;
			hr = pPins->QueryInterface(IID_IAMStreamConfig, (void**)&streamCfg);
			if (SUCCEEDED(hr)){
				int count = 0;
				int size = 0;
				VIDEO_STREAM_CONFIG_CAPS *pCaps = NULL;

				hr = streamCfg->GetNumberOfCapabilities(&count, &size);
				if (hr != S_OK){
					continue;
				}
				if (size != sizeof(VIDEO_STREAM_CONFIG_CAPS)){
					continue;
				}

				pCaps = new VIDEO_STREAM_CONFIG_CAPS;
				int idx = 0;
				for (int i = 0; i < count; i++){
					AM_MEDIA_TYPE *pmt = NULL;
					if (streamCfg->GetStreamCaps(i, &pmt, (BYTE*)pCaps) == S_OK){
						addOutputFormat(pmt, pCaps);

						dxUtils::FreeMediaType(*pmt);
					}
				}
				delete pCaps;
				streamCfg->Release();
			}
		}
		pPins->Release();
	}
	pEnum->Release();

done:
	return hr;
}

DEVCAPOUTPUTFMT* DeviceList::getDeviceSupportOutputFormat(size_t& index)
{
	index = devOutputFormats.size();
	return devOutputFormats.data();
}

HRESULT DeviceList::enumDevices()
{
	HRESULT hr = S_OK;
	ICreateDevEnum *pSystemEnum = NULL;
	IEnumMoniker *pEnumMoniker = NULL;
	
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSystemEnum);
	if (hr != S_OK){
		//
		goto done;
	}

	hr = pSystemEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	if (hr != S_OK){
		//
		goto done;
	}

	ULONG ret = 0;
	IMoniker *pDeviceInfo = NULL;

	while (pEnumMoniker->Next(1, &pDeviceInfo, &ret) == S_OK){
		IPropertyBag *propertysInfo = NULL;
		VARIANT friendlyName;
		VARIANT devPath;
		VariantInit(&friendlyName);
		VariantInit(&devPath);
		
		hr = pDeviceInfo->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)&propertysInfo);
		if (hr == S_OK){
			DEVDESCRIPT dev;
			hr = propertysInfo->Read(TEXT("FriendlyName"), &friendlyName, 0);
			if (SUCCEEDED(hr)){
				dev.name = friendlyName.bstrVal;
				VariantClear(&friendlyName);
			}
			
			hr = propertysInfo->Read(TEXT("DevicePath"), &devPath, 0);
			if (SUCCEEDED(hr)){
				dev.path = devPath.bstrVal;
				VariantClear(&devPath);
			}

			m_devices.push_back(dev);
			propertysInfo->Release();
		}
		pDeviceInfo->Release();
	}

done:
	if (pSystemEnum){
		pSystemEnum->Release();
	}

	if (pEnumMoniker){
		pEnumMoniker->Release();
	}

	return hr;
}

DEVDESCRIPT *DeviceList::getDevicesDesc(size_t &count) {
	count = m_devices.size();
	return m_devices.data();
}

IBaseFilter* DeviceList::getDevice(int index)
{
	IBaseFilter * requestFilter = NULL;

	HRESULT hr = S_OK;
	ICreateDevEnum *pSystemEnum = NULL;
	IEnumMoniker *pEnumMoniker = NULL;
	DEVDESCRIPT requestDev;

	if (index > m_devices.size()){
		goto done;
	}

	requestDev = m_devices[index];

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSystemEnum);
	if (hr != S_OK){
		//
		goto done;
	}

	hr = pSystemEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	if (hr != S_OK){
		//
		goto done;
	}

	ULONG ret = 0;
	IMoniker *pDeviceInfo = NULL;

	while (pEnumMoniker->Next(1, &pDeviceInfo, &ret) == S_OK){
		IPropertyBag *propertysInfo = NULL;
		VARIANT friendlyName;
		VARIANT devPath;
		VariantInit(&friendlyName);
		VariantInit(&devPath);

		hr = pDeviceInfo->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)&propertysInfo);
		if (hr == S_OK){
			DEVDESCRIPT dev;
			hr = propertysInfo->Read(TEXT("FriendlyName"), &friendlyName, 0);
			if (SUCCEEDED(hr)){
				dev.name = friendlyName.bstrVal;
				VariantClear(&friendlyName);
			}

			hr = propertysInfo->Read(TEXT("DevicePath"), &devPath, 0);
			if (SUCCEEDED(hr)){
				dev.path = devPath.bstrVal;
				VariantClear(&devPath);
			}

			if ((dev.name == requestDev.name) && (dev.path == requestDev.path)){
				hr = pDeviceInfo->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&requestFilter);
				enumPins(requestFilter);
				if (SUCCEEDED(hr)){
					propertysInfo->Release();
					pDeviceInfo->Release();
					break;
				}
			}
			
			propertysInfo->Release();
		}
		pDeviceInfo->Release();
	}

done:
	if (pSystemEnum){
		pSystemEnum->Release();
	}

	if (pEnumMoniker){
		pEnumMoniker->Release();
	}
	return requestFilter;
}
