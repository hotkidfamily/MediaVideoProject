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
}DEVINFO;

class DeviceList
{
public:
	DeviceList();
	~DeviceList();

	size_t count() const { return m_devices.size(); }
	DEVINFO *getDevices() { return m_devices.data(); }
	HRESULT enumDevices();
	DEVCAPOUTPUTFMT* getDevicesSupportOutputFormat(int index);
	IBaseFilter * getDevice(int index);

private:
	HRESULT enumPins(IBaseFilter *captureFilter);
	void addOutputFormat(AM_MEDIA_TYPE *pvh, VIDEO_STREAM_CONFIG_CAPS* pcaps);

	std::vector<DEVINFO> m_devices;
	std::vector<DEVCAPOUTPUTFMT> devOutputFormats;
};

