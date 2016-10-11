#include "stdafx.h"
#include "DeviceList.h"

DeviceList::DeviceList(logger& log)
	: log(log)
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
	if (output.type == VideoOutputType_None){
		return;
	}

	output.outputRes.cx = pvh->biWidth;
	output.outputRes.cy = abs(pvh->biHeight);
	if (pCaps){
		output.minFrameInterval = pCaps->MinFrameInterval;
		output.maxFrameInterval = pCaps->MaxFrameInterval;
	}else{
		output.minFrameInterval = output.maxFrameInterval = *(dxUtils::getFrameInterval(pmt));
	}

	m_pinOutputMap[output.type].push_back(output);
}

HRESULT DeviceList::queryPinOutputFormat(IPin *pin)
{
	HRESULT hr = E_FAIL;
	IEnumMediaTypes *pMediaTypeEnum = NULL;
	VIDEO_STREAM_CONFIG_CAPS *pCaps = NULL;
	IAMStreamConfig *streamCfg = NULL;
	AM_MEDIA_TYPE *pmt = NULL;
	int idx = 0;

	//pin->Disconnect();
	if (SUCCEEDED(hr = pin->QueryInterface(IID_IAMStreamConfig, (void**)&streamCfg))){
		int count = 0, size = 0;
		if (SUCCEEDED(hr = streamCfg->GetNumberOfCapabilities(&count, &size))){
			pCaps = new VIDEO_STREAM_CONFIG_CAPS;

			for (int i = 0; i < count; i++){
				if (streamCfg->GetStreamCaps(i, &pmt, (BYTE*)pCaps) == S_OK){
					addOutputFormat(pmt, pCaps);
					BITMAPINFOHEADER * pvh = dxUtils::getBmpHeader(pmt);
					log.log(1, TEXT("\t\t %3d %4dx%4d fps: %f ~ %f (%s,%s,%s)\n"), idx++, pvh->biWidth, abs(pvh->biHeight),
						dxUtils::calcFps(pCaps->MaxFrameInterval), dxUtils::calcFps(pCaps->MinFrameInterval), dxUtils::getInfo(pmt->majortype, UNUSED_FOURCC_CODE),
						dxUtils::getInfo(pmt->subtype, pvh->biCompression), dxUtils::getInfo(pmt->formattype, UNUSED_FOURCC_CODE));
					dxUtils::FreeMediaType(*pmt);
				}
			}
			delete pCaps;
		}

		streamCfg->Release();
	}else if (SUCCEEDED(hr = pin->EnumMediaTypes(&pMediaTypeEnum))){
		while (pMediaTypeEnum->Next(1, &pmt, NULL) == S_OK){
			//log.
			addOutputFormat(pmt, pCaps);
			dxUtils::DeleteMediaType(pmt);
		}

		pMediaTypeEnum->Release();
	}

	return hr;
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
		PIN_DIRECTION PinDir;
		if (pPins->QueryDirection(&PinDir) == S_OK){
			if (PinDir != PINDIR_OUTPUT){
				pPins->Release();
				continue;
			}

			queryPinOutputFormat(pPins);
		}
		pPins->Release();
	}
	pEnum->Release();

done:
	return hr;
}

std::map<int, std::vector<DEVCAPOUTPUTFMT>> DeviceList::getDeviceSupportOutputFormat()
{
	return m_pinOutputMap;
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

	if (index > (int)m_devices.size()){
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
