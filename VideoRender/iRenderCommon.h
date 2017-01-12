#pragma  once

class IRenderThread
{
public:
	virtual DWORD RenderLoop() = 0;
};