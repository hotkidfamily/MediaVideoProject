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

bool DeviceList::isCapturePin(IPin *pPin)
{
	IKsPropertySet *pKs = NULL;
	HRESULT hr;
	DWORD cbReturned = 0;
	GUID pinCat = {0};

	if (SUCCEEDED(hr = pPin->QueryInterface(IID_PPV_ARGS(&pKs)))){
		hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,
			&pinCat, sizeof(GUID), &cbReturned);
		pKs->Release();
	}

	return (pinCat == PIN_CATEGORY_CAPTURE);
}


HRESULT DeviceList::showPinPorpertyPages(IPin *pin)
{
	HRESULT hr;
	ISpecifyPropertyPages *pSpec;
	CAUUID cauuid;
	IAMStreamConfig *pSC = NULL;

	if (isCapturePin(pin)){
		if (SUCCEEDED(hr = pin->QueryInterface(IID_IAMStreamConfig, (void**)&pSC))){
			
			if (SUCCEEDED(hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec))){
				hr = pSpec->GetPages(&cauuid);
				hr = OleCreatePropertyFrame(NULL, 30, 30, NULL, 1,
					(IUnknown **)&pSC, cauuid.cElems,
					(GUID *)cauuid.pElems, 0, 0, NULL);

				CoTaskMemFree(cauuid.pElems);
				pSpec->Release();
			}

			pSC->Release();
		}
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
		}
		pPins->Release();
	}
	pEnum->Release();

done:
	return hr;
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
