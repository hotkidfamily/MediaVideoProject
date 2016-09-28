#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <dshow.h>
#include "logger.h"

typedef struct tagDevDesc{
	STRING name;
	STRING path;
	int nOutput;
	void *outputs;
}DEVINFO;

class DeviceList
{
public:
	DeviceList();
	~DeviceList();

	size_t count() const { return m_devices.size(); }
	DEVINFO *getDevices() { return m_devices.data(); }
	HRESULT enumDevices();
	IBaseFilter * getDevice(int index);

private:
	std::vector<DEVINFO> m_devices;
};

