
// War3ToolDlg.h: 头文件
//

#pragma once


// CWar3ToolDlg 对话框
class CWar3ToolDlg : public CDialogEx
{
// 构造
public:
	CWar3ToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAR3TOOL_DIALOG };
#endif

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
	afx_msg void OnBnClickedAddMoney();
	afx_msg void OnBnClickedAddKillCount();
	afx_msg void OnBnClickedAddSoul();
	afx_msg HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
	afx_msg BOOL CWar3ToolDlg::ModuleAddress(DWORD pid, wchar_t* ModuleName, QWORD& StartAddress, QWORD& EndAddress);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_time;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedXHeroAddMoney();
};
