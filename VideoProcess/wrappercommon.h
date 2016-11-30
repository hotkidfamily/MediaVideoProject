#ifndef __WRAPPERCOMMON_H__
#define __WRAPPERCOMMON_H__

#define FUNCTION_DECLARE(return_value, func_name, params_list)\
	typedef return_value (*P##func_name) params_list ;\
	return_value func_name params_list;

class DwWrapperCommon {

public:
	DwWrapperCommon(const wchar_t *pFileName);

	~DwWrapperCommon();
	
	BOOL DwGetProcAddr(FARPROC *func, LPCSTR pFunctionName);

private:
	HMODULE m_library;
};

#endif //__WRAPPERCOMMON_H__