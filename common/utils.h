#ifndef __VCUTILS_H__
#define __VCUTILS_H__

#include <string>

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

typedef struct tagCOMError{
	HRESULT hr;
	const TCHAR* desc;
}COMERROR;

#define COMERROR2STR(x) {x, TEXT(#x)}

#endif //__VCUTILS_H__