#pragma  once
#include "IVideoCodec.h"

class ICodecFactoryImpl :public ICodecFactory
{
public:
	ICodecFactoryImpl();
	~ICodecFactoryImpl();

	BOOL CreateCodecObj(ICodec* &);
	BOOL DestoryCodecObj(ICodec *);

protected:
	BOOL UpdateCodecAbilityTable();
};