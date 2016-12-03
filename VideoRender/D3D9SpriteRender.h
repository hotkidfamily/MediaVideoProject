#pragma once

#include "IRender.h"

class D3D9SpriteRender : public IRender
{
public:
	D3D9SpriteRender();
	~D3D9SpriteRender();

public:
	virtual BOOL Repaint() { return TRUE; }
	virtual BOOL InitRender(HWND, int32_t, int32_t, DWORD);
	virtual BOOL DeinitRender();

	virtual BOOL PushFrame(CSampleBuffer *frame);
	virtual const char *GetRenderDescriptor() const { return "D3D9 sprite render"; };
};

