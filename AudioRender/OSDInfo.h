#pragma once

/*
 * // common information for OSD
 */

class OSDInfo
{
public:
	OSDInfo();
	~OSDInfo();
	TCHAR *GetOSDInfo();

private:
	uint32_t m_jitterFrameCount; 
	uint32_t m_pushFrameCount;
	uint32_t m_popFrameCount;
	BOOL m_bSoftwareConvert;

	TCHAR m_OSDInfo[2048];
};

