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
			DEVINFO dev;
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

IBaseFilter* DeviceList::getDevice(int index)
{
	IBaseFilter * requestFilter = NULL;


	HRESULT hr = S_OK;
	ICreateDevEnum *pSystemEnum = NULL;
	IEnumMoniker *pEnumMoniker = NULL;
	DEVINFO requestDev;

	if (index > count()){
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
			DEVINFO dev;
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
