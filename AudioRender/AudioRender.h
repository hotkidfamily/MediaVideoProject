#pragma once

#include "IAduioRender.h"

class IAduioRenderFactoryImpl :
	public IAudioRenderFactory
{
public:
	IAduioRenderFactoryImpl();
	~IAduioRenderFactoryImpl();

	BOOL CreateAudioRenderObj(IAudioRender *&);
	BOOL DestoryAudioRenderObj(IAudioRender *);
};