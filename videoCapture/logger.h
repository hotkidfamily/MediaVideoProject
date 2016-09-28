#pragma once

#include "stdafx.h"

#include <fstream>

class logger
{
public:
	logger(STRING filePath);
	~logger();

	void log(int level, const TCHAR *format, ...);

private:
#ifdef UNICODE
	std::wofstream logfile;
#else
	std::ofstream logfile;
#endif
};

