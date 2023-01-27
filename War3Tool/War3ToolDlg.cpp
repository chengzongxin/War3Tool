
// War3ToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "War3Tool.h"
#include "War3ToolDlg.h"
#include "afxdialogex.h"


#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <dbghelp.h>
#include <string>
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Dbghelp.lib")
#include <stdio.h>

#include "WriteOffsetMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWar3ToolDlg 对话框



CWar3ToolDlg::CWar3ToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WAR3TOOL_DIALOG, pParent)
	, m_inputData(_T(""))
	, m_noPauseTimes(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWar3ToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_time);
	DDX_Text(pDX, IDC_EDIT2, m_inputData);
	DDX_Check(pDX, IDC_CHECK1, m_noPauseTimes);
}

BEGIN_MESSAGE_MAP(CWar3ToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CWar3ToolDlg::OnBnClickedAddMoney)
	ON_BN_CLICKED(IDC_BUTTON3, &CWar3ToolDlg::OnBnClickedAddKillCount)
	ON_BN_CLICKED(IDC_BUTTON2, &CWar3ToolDlg::OnBnClickedAddSoul)
	//	ON_WM_TIMER()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON4, &CWar3ToolDlg::OnBnClickedXHeroAddMoney)
	ON_BN_CLICKED(IDC_CHECK1, &CWar3ToolDlg::OnBnClickedPauseLimitless)
END_MESSAGE_MAP()


// CWar3ToolDlg 消息处理程序

BOOL CWar3ToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 100, NULL);   //编号为1,时间周期为100ms,第三个参数回调函数，设为NULL即可
	m_noPauseTimes = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWar3ToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWar3ToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWar3ToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWar3ToolDlg::OnBnClickedAddMoney()
{
	// TODO: 加钱
	// "Storm.dll"+0x0002BC58
	// 44
	// 7F8
	// 1E4
	// 1C
	// AC
	DWORD arr[5] = { 0x44, 0x7F8, 0x1E4, 0x1C, 0xAC };
	DWORD data = GetDlgItemInt(IDC_EDIT2, NULL, 1);
	DWORD length = sizeof(arr) / sizeof(arr[0]);
	WriteOffsetMemory::Write(_T("Storm.dll"), 0x0002BC58, arr, length, data);
}

void CWar3ToolDlg::OnBnClickedAddSoul()
{
	// TODO: 加灵魂
	// "Storm.dll"+0002BC58
	// 44
	// 7F8
	// 214
	// 1C
	// 5C
	DWORD arr[5] = { 0x44, 0x7F8, 0x214, 0x1C, 0x5C };
	DWORD data = GetDlgItemInt(IDC_EDIT2, NULL, 1);
	DWORD length = sizeof(arr) / sizeof(arr[0]);
	WriteOffsetMemory::Write(_T("Storm.dll"), 0x0002BC58, arr, length, data);
}

void CWar3ToolDlg::OnBnClickedAddKillCount()
{
	// TODO: 加杀敌数
	//"Storm.dll" + 0002BC58
	//44
	//158
	//24
	//138
	//514
	DWORD arr[5] = { 0x44, 0x158, 0x24, 0x138, 0x514 };
	DWORD data = GetDlgItemInt(IDC_EDIT2, NULL, 1);
	DWORD length = sizeof(arr) / sizeof(arr[0]);
	WriteOffsetMemory::Write(_T("Storm.dll"), 0x0002BC58, arr, length, data);
}

void CWar3ToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);

	switch (nIDEvent)
	{
	case 1:  //编号一的定时器
		SYSTEMTIME st;
		CString strDate, strTime;
		//获取系统时间
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
		strTime.Format(_T("%2d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
		CString show_time = strDate + " " + strTime;
		//设置字体
		CFont m_editFont;
		m_editFont.CreatePointFont(120, _T("宋体"), NULL);
		m_time.SetFont(&m_editFont, TRUE);   //m_times 为显示控件的变量名
		m_time.SetWindowText(show_time);

		// 1秒加一次钱
		OnBnClickedAddMoney();
		OnBnClickedAddSoul();
		OnBnClickedAddKillCount();
		OnBnClickedPauseLimitless();

		break;
	}

	//3.销毁定时器
	//在程序退出前需要销毁定时器，添加下面代码
	//KillTimer(1); //1为定时器的编号
}

void CWar3ToolDlg::OnBnClickedXHeroAddMoney()
{
	// "Game.dll"+0x00BE40A4
	// 4
	// 14
	// 3c
	// 78
	DWORD arr[4] = { 0x4, 0x14, 0x3c, 0x78 };
	DWORD data = GetDlgItemInt(IDC_EDIT2, NULL, 1);
	DWORD length = sizeof(arr) / sizeof(arr[0]);
	WriteOffsetMemory::Write(_T("Game.dll"), 0x00BE40A4, arr, length, data);
}


void CWar3ToolDlg::OnBnClickedPauseLimitless()
{
	// TODO: 暂停不限制次数
	UpdateData(TRUE);
	if (m_noPauseTimes)
	{
		//"Storm.dll" + 0x0002BC58
		//	0x1C4
		//	0x0
		//	0x0
		//	0x1C
		//	0x40
		DWORD arr[5] = { 0x1C4, 0x0, 0x0, 0x1C, 0x40 };
		DWORD data = 0;
		DWORD length = sizeof(arr) / sizeof(arr[0]);
		WriteOffsetMemory::Write(_T("Storm.dll"), 0x0002BC58, arr, length, data, TRUE);
	}
}
