#pragma  once
#include "IVPP.h"

class IVPPFactoryImpl : public IVPPFactory
{
public:
	IVPPFactoryImpl();
	~IVPPFactoryImpl();

	IVPP* IVPPFactoryImpl::CreateVPP();
	void IVPPFactoryImpl::DestoryVPP(IVPP *ctx);

private:
};

