#pragma  once
#include "IVPP.h"

class IVPPFactoryImpl : public IVPPFactory
{
public:
	IVPPFactoryImpl();
	~IVPPFactoryImpl();

	BOOL IVPPFactoryImpl::CreateVPPObj(IVPP *&);
	void IVPPFactoryImpl::DestoryVPPObj(IVPP *);

private:
};

