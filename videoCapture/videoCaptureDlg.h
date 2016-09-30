
// videoCaptureDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "DeviceList.h"
#include "dxVideoCapture.h"

// CvideoCaptureDlg �Ի���
class CvideoCaptureDlg : public CDialogEx
{
// ����
public:
	CvideoCaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VIDEOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_devicesCombox;

private:
	DeviceList *m_devicesList;
	dxVideoCapture *m_videoCap;
	logger *log;

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonOpen();
	CButton m_openButton;
	CStatic m_videoWindow;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
