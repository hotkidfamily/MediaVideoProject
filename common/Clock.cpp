#include "stdafx.h"
#include "Clock.h"

CClock::CClock()
	: m_baseRefTimeIn100ns(0)
	, m_baseRefTimeDiff(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_llPerfFrequency);
	m_baseRefTimeIn100ns = GetCurrentTimeIn100ns();
}

CClock::~CClock()
{
}

LONGLONG CClock::GetBaseTime()
{
	return m_baseRefTimeIn100ns;
}

BOOL CClock::ResetBaseTime(LONGLONG baseRefTimeIn100ns)
{
	m_baseRefTimeIn100ns = GetCurrentTimeIn100ns();
	m_baseRefTimeDiff = baseRefTimeIn100ns - m_baseRefTimeIn100ns;
	return TRUE;
}

LONGLONG CClock::GetCurrentTimeInMs() // millisecond
{
	return trans100ns2ms(GetCurrentTimeIn100ns());
}

LONGLONG CClock::GetCurrentTimeIn100ns() // 100 nano second 
{
	LONGLONG i64Ticks100ns = 0;
	if (m_llPerfFrequency != 0) {
		QueryPerformanceCounter((LARGE_INTEGER*)&i64Ticks100ns);
		i64Ticks100ns = LONGLONG((double(i64Ticks100ns) * 10000000) / double(m_llPerfFrequency) + 0.5);
		return i64Ticks100ns + m_baseRefTimeDiff;
	}
	return 0;
}