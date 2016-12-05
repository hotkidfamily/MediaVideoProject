#ifndef __VCUTILS_H__
#define __VCUTILS_H__

#include <string>
#include <stdint.h>

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

#ifdef __cplusplus
}
#endif

typedef struct tagCOMError{
	HRESULT hr;
	const TCHAR* desc;
}COMERROR;

#define COMERROR2STR(x) {x, TEXT(#x)}

#endif //__VCUTILS_H__