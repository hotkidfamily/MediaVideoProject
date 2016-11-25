#ifndef SAFE_DELETE
#define SAFE_DELETE(x)  {if(x) delete (x); (x)= NULL;}
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
#define SAFE_RELEASE(x) { if(x) x->Release(); x=NULL; }
#endif