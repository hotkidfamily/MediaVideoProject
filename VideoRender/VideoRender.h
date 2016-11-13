#include "IRender.h"

class IRenderDelegate :
	public IRender
{
public:
	HRESULT Repaint();
	HRESULT InitRender();
	HRESULT DeinitRender();

	HRESULT PushFrame(RENDER_FRAME &frame);
	HRESULT GetRenderNameStr(const char* &);
protected:
private:
};