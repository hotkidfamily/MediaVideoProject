#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <map>
#include <dshow.h>
#include "logger.h"
#include "dxUtils.h"

typedef struct tagDevCaptureOutputFormat{
	int type;
	SIZE outputRes;
	REFERENCE_TIME minFrameInterval;
	REFERENCE_TIME maxFrameInterval;
}DEVCAPOUTPUTFMT;

typedef struct tagDevDesc{
	STRING name;
	STRING path;
}DEVDESCRIPT;

class DeviceList
{
public:
	DeviceList();
	~DeviceList();

	HRESULT enumDevices();

	DEVDESCRIPT *getDevicesDesc(size_t &count);

	IBaseFilter * getDevice(int index);
	DEVCAPOUTPUTFMT* getDeviceSupportOutputFormat(size_t &count);

private:
	HRESULT enumPins(IBaseFilter *captureFilter);
	void addOutputFormat(AM_MEDIA_TYPE *pvh, VIDEO_STREAM_CONFIG_CAPS* pcaps);

	std::vector<DEVDESCRIPT> m_devices;
	std::vector<DEVCAPOUTPUTFMT> devOutputFormats;
};

