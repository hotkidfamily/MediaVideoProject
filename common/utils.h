#ifndef __COMMONUTILS_H__
#define __COMMONUTILS_H__

#include <string>
#include <stdint.h>
#include "PixelFormat.h"
#include "SampleBuffer.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)  {if(x) delete (x); (x)= nullptr;}
#endif

#ifdef UNICODE
#define STRING std::wstring
#else
#define STRING std::string
#endif

#ifndef CHECK_HR
#define CHECK_HR(hr) if (FAILED(hr)) { goto done; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if(x) x->Release(); x=nullptr; }
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// align width & height by 32 and 16
	#define WIDTHALIGN(x) ((x + 0x1F) & (~0x1F))
	#define HEIGHTALIGN(x) ((x + 0xF) & (~0xF))

	#define MAX_NB_PLANAR (4)

	void DeallocMemory(uint8_t * pointerOfMemory);
	uint8_t * AllocMemory(uint32_t sizeInByte);
	bool ReallocMemory(uint8_t** buffer, uint32_t sizeInByte);

	BOOL DeallocSampleBuffer(CSampleBuffer *);
	CSampleBuffer *AllocSampleBuffer(int32_t, int32_t, CPPixelFormat);

#ifdef __cplusplus
}
#endif

#endif //__COMMONUTILS_H__