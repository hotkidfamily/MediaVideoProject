#pragma once

#ifdef VIDEOPROCESS_EXPORTS
#define VIDEOPROCESS_API __declspec(dllexport)
#else
#define VIDEOPROCESS_API __declspec(dllimport)
#endif

class VPPFactory
{
public:
	VPPFactory();
	~VPPFactory();

	BOOL CreateVPP();
	BOOL DestoryVPP();
};

class IVPP {
public:
	virtual BOOL InitContext() = 0;
	virtual BOOL DeinitContext() = 0;

	virtual BOOL ProcessFrame() = 0;

protected:
	~IVPP(){};
};

extern "C" {

	VIDEOPROCESS_API  VPPFactory *GetVPPFactoryObj();
	VIDEOPROCESS_API void ReleaseVPPFctoryObj();

}
