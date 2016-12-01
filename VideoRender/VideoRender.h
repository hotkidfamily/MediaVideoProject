#pragma once

#include "IRender.h"

class IRenderFactoryImpl :
	public IRenderFactory
{
public:
	IRenderFactoryImpl();
	~IRenderFactoryImpl();

	BOOL CreateRenderObj(IRender *);
	BOOL DestoryRenderObj(IRender *);
};