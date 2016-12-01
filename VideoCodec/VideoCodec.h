#pragma  once
#include "IVideoCodec.h"

class ICodecFactoryImpl :public ICodecFactory
{
public:
	ICodecFactoryImpl();
	~ICodecFactoryImpl();

	BOOL CreateCodec(ICodec* &codec);
	BOOL DestoryCodec(ICodec *&codec);

protected:
	BOOL UpdateCodecAbilityTable();
};