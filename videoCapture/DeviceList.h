#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <map>
#include <dshow.h>
#include "logger.h"
#include "dxUtils.h"
#include "Pins.h"


typedef struct tagDevDesc{
	STRING name;
	STRING path;
}DEVDESCRIPT;

class DeviceList
{
public:
	DeviceList(logger &log);
	~DeviceList();

	HRESULT enumDevices();

	DEVDESCRIPT *getDevicesDesc(size_t &count);

	IBaseFilter * getDevice(int index);
	std::map<int, std::vector<DEVCAPOUTPUTFMT>> getDeviceSupportOutputFormat();
	HRESULT showPinPorpertyPages(IPin *);

private:
	HRESULT enumPins(IBaseFilter *captureFilter);
	HRESULT queryPinOutputFormat(IPin *pin);
	void addOutputFormat(AM_MEDIA_TYPE *pvh, VIDEO_STREAM_CONFIG_CAPS* pcaps);
	bool isCapturePin(IPin *pPin);

	std::vector<DEVDESCRIPT> m_devices;
private:
	logger &log;
};

