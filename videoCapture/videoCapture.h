
// videoCapture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CvideoCaptureApp: 
// �йش����ʵ�֣������ videoCapture.cpp
//

class CvideoCaptureApp : public CWinApp
{
public:
	CvideoCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CvideoCaptureApp theApp;