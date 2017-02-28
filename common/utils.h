#ifndef __COMMONUTILS_H__
#define __COMMONUTILS_H__

#include "stdafx.h"
#include "PixelFormat.h"
#include "SampleBuffer.h"


//copy from http://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t

typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> W2S, S2W;

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


// align width & height by 32 and 16
#define WIDTHALIGN(x) ((x + 0x1F) & (~0x1F))
#define HEIGHTALIGN(x) ((x + 0xF) & (~0xF))

#define MAX_NB_PLANAR (4)

void DeallocMemory(uint8_t * pointerOfMemory);
uint8_t * AllocMemory(uint32_t sizeInByte);
bool ReallocMemory(uint8_t** buffer, uint32_t sizeInByte);

BOOL DeallocSampleBuffer(CSampleBuffer *);
CSampleBuffer *AllocSampleBuffer(int32_t, int32_t, CPPixelFormat);

DWORD GetFourCCByPixFmt(int format);
int32_t GetPixFmtByFourCC(DWORD pfFourCC);

#endif //__COMMONUTILS_H__