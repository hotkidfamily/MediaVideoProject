
// videoCaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "videoCapture.h"
#include "videoCaptureDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CvideoCaptureDlg �Ի���



CvideoCaptureDlg::CvideoCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvideoCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvideoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEIVCES, m_devicesCombox);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_openButton);
	DDX_Control(pDX, IDC_VIDEO_VIEW, m_videoWindow);
	DDX_Control(pDX, IDC_COMBO_DATAFORMAT, m_videoOutputFormatCombox);
	DDX_Control(pDX, IDC_COMBO_RES, m_videoResCombox);
	DDX_Control(pDX, IDC_COMBO_FPS, m_videoFpsCombox);
}

BEGIN_MESSAGE_MAP(CvideoCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CvideoCaptureDlg::OnBnClickedButtonOpen)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_DEIVCES, &CvideoCaptureDlg::OnCbnSelchangeComboDeivces)
	ON_CBN_SELCHANGE(IDC_COMBO_DATAFORMAT, &CvideoCaptureDlg::OnCbnSelchangeComboDataformat)
END_MESSAGE_MAP()


// CvideoCaptureDlg ��Ϣ�������

BOOL CvideoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	log = new logger(TEXT("c:\\this.txt"));
	m_devicesList = new DeviceList(*log);
	m_devicesList->enumDevices();
	m_videoCap = new dxVideoCapture(*log);
	m_videoCap->initGraph(m_videoWindow.m_hWnd);

	size_t deviceCnt = 0;
	DEVDESCRIPT *ppdevices = m_devicesList->getDevicesDesc(deviceCnt);

	for (int i = 0; i < (int)deviceCnt; i++){
		m_devicesCombox.AddString(ppdevices[i].name.c_str());
	}
	m_devicesCombox.SetCurSel(0);

	OnCbnSelchangeComboDeivces();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CvideoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CvideoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CvideoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CvideoCaptureDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	delete m_devicesList;
	// TODO:  �ڴ˴������Ϣ����������
}


void CvideoCaptureDlg::OnBnClickedButtonOpen()
{
	if (m_videoCap->isRuning()){
		m_videoCap->stop();
		m_openButton.SetWindowTextW(TEXT("Open"));
	}
	else{
		//	m_videoCap->showFilterPropertyPages(capture);
		
		m_videoCap->buildGraph(captureFilter);
		m_videoCap->start();
		m_openButton.SetWindowTextW(TEXT("Close"));
	}

}


void CvideoCaptureDlg::OnSize(UINT nType, int cx, int cy)
{
	// TODO:  �ڴ˴������Ϣ����������
	if (m_videoCap->isRuning())
		m_videoCap->ResizeVideoWindow();
}

#include <iterator>

void CvideoCaptureDlg::OnCbnSelchangeComboDeivces()
{
	std::map<int, std::vector<DEVCAPOUTPUTFMT>> pFmt;
	captureFilter = m_devicesList->getDevice(m_devicesCombox.GetCurSel());
	pFmt = m_devicesList->getDeviceSupportOutputFormat();
	int count = 0;

	m_videoOutputFormatCombox.ResetContent();
	m_videoResCombox.ResetContent();
	m_videoFpsCombox.ResetContent();

	std::map<int, std::vector<DEVCAPOUTPUTFMT>>::reverse_iterator iterator;
	for (iterator = pFmt.rbegin(); iterator != pFmt.rend(); iterator++){
		CString str;
		str.Format(TEXT("%d"), count++);
		m_videoOutputFormatCombox.AddString(str);
		std::vector<DEVCAPOUTPUTFMT>::iterator it;
		for (it = iterator->second.begin(); it != iterator->second.end(); it++){
			CString res, fps;
			res.Format(TEXT("%dx%d"), it->outputRes.cx, it->outputRes.cy);
			fps.Format(TEXT("%fx%f"), dxUtils::calcFps(it->maxFrameInterval), dxUtils::calcFps(it->minFrameInterval));
			m_videoResCombox.AddString(res);
			m_videoFpsCombox.AddString(fps);
		}
	}

	m_videoOutputFormatCombox.SetCurSel(0);
	m_videoResCombox.SetCurSel(0);
	m_videoFpsCombox.SetCurSel(0);

}


void CvideoCaptureDlg::OnCbnSelchangeComboDataformat()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
