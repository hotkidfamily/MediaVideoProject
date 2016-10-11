
// videoCaptureDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "DeviceList.h"
#include "dxVideoCapture.h"

// CvideoCaptureDlg 对话框
class CvideoCaptureDlg : public CDialogEx
{
// 构造
public:
	CvideoCaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VIDEOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	IBaseFilter *captureFilter;
	logger *log;

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonOpen();
	CButton m_openButton;
	CStatic m_videoWindow;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboDeivces();
	CComboBox m_videoOutputFormatCombox;
	CComboBox m_videoResCombox;
	CComboBox m_videoFpsCombox;
	afx_msg void OnCbnSelchangeComboDataformat();
};
