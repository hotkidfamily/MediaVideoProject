#include "stdafx.h"

#include "wrapperCommon.h"

DwWrapperCommon::DwWrapperCommon(const wchar_t *pFileName)
{
    m_library = LoadLibraryEx(pFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
}

DwWrapperCommon::~DwWrapperCommon()
{
	if (m_library)
	{
		FreeLibrary(m_library);
		m_library = NULL;
	}
}

BOOL DwWrapperCommon::DwGetProcAddr(FARPROC * func, LPCSTR pFunctionName)
{
	if (m_library)
	{
		if(*func == NULL)
			*func = ::GetProcAddress(m_library, pFunctionName);
	}
	else
	{
		*func = NULL;
	}

	return (*func != NULL);
}
