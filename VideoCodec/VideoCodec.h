#pragma  once
#include "IVideoCodec.h"

class ICodecFactoryImpl :public ICodecFactory
{
public:
	ICodecFactoryImpl();
	~ICodecFactoryImpl();

	BOOL CreateCodecObj(ICodec* &codec);
	BOOL DestoryCodecObj(ICodec *&codec);

protected:
	BOOL UpdateCodecAbilityTable();
};