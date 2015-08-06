
// SortSystemDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "DlgSortSystemOperater.h"
#include "DlgSortSystemParameter.h"


// CSortSystemDlg 对话框
class CSortSystemDlg : public CDialogEx
{
// 构造
public:
	CSortSystemDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SORTSYSTEM_DIALOG };

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
	afx_msg void OnBnClickedButton1();
	CDlgSortSystemOperater m_Operater;
	CDlgSortSystemParameter m_Parameter;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickedButtonParam();
	afx_msg void OnBnClickedButtonOperate();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
