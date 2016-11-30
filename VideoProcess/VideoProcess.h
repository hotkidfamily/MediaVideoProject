#pragma  once

#include "IVPP.h"

class VPPFactory: public IVPP
{
public:
	VPPFactory();
	~VPPFactory();

	virtual BOOL open();
	virtual BOOL close();
	virtual BOOL process();

protected:

private:

};