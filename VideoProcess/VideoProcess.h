#pragma  once
#include "IVPP.h"

class IVPPFactoryImpl : public IVPPFactory
{
public:
	IVPPFactoryImpl();
	~IVPPFactoryImpl();

	IVPP* IVPPFactoryImpl::CreateVPPObj();
	void IVPPFactoryImpl::DestoryVPPObj(IVPP *ctx);

private:
};

