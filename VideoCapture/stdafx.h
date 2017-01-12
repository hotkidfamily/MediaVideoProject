// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdint.h>
#include <tchar.h>

#include <string>
#include <list>
#include <vector>

#include <DShow.h>
#include <dvdmedia.h>

#include <atlbase.h>

#include "utils.h"
#include "AutoLock.h"

#ifdef UNICODE
#define STRING std::wstring
#else
#define STRING std::string
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)  {if(x) delete (x); (x)= nullptr;}
#endif
