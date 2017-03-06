#pragma once

#include "IAduioRender.h"

class IAduioRenderFactoryImpl :
	public IAudioRenderFactory
{
public:
	IAduioRenderFactoryImpl();
	~IAduioRenderFactoryImpl();

	BOOL CreateRenderObj(IAudioRender *&);
	BOOL DestoryRenderObj(IAudioRender *);
};