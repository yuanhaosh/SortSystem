#pragma once


// CDlgSortSystemLoad 对话框

class CDlgSortSystemLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemLoad)

public:
	CDlgSortSystemLoad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSortSystemLoad();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad();
};
