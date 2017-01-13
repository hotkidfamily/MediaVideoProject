#include "stdafx.h"
#include "Clock.h"

CClock::CClock()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_llPerfFrequency);
}

CClock::~CClock()
{
}

LONGLONG CClock::GetCurrentTimeInMs() // millisecond
{
	LONGLONG i64Ticks100ns = 0;
	if (m_llPerfFrequency != 0) {
		QueryPerformanceCounter((LARGE_INTEGER*)&i64Ticks100ns);
		i64Ticks100ns = LONGLONG((double(i64Ticks100ns) * 10000000) / double(m_llPerfFrequency) + 0.5);
		return trans100ns2ms(i64Ticks100ns);
	}
	return 0;
}

LONGLONG CClock::GetCurrentTimeIn100ns() // 100 nano second 
{
	LONGLONG i64Ticks100ns = 0;
	if (m_llPerfFrequency != 0) {
		QueryPerformanceCounter((LARGE_INTEGER*)&i64Ticks100ns);
		i64Ticks100ns = LONGLONG((double(i64Ticks100ns) * 10000000) / double(m_llPerfFrequency) + 0.5);
		return i64Ticks100ns;
	}
	return 0;
}