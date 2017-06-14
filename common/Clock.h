#pragma once

#include <stdint.h>

// REFERENCE_TIME == LONGLONG

inline LONGLONG trans100ns2ms(LONGLONG nano)
{
	return nano / 10000;
}

class CClock
{
public:
	CClock();
	~CClock();

	LONGLONG GetBaseTime();
	BOOL ResetBaseTime(LONGLONG);
	LONGLONG GetCurrentTimeInMs();
	LONGLONG GetCurrentTimeIn100ns();
	LONGLONG MapTimeToLoacl(uint32_t time);

private:
	LONGLONG m_llPerfFrequency;
	LONGLONG m_baseRefTimeIn100ns;
	LONGLONG m_baseRefTimeDiff;
	
	BOOL m_bMap;
};

