/*******************************************************************************************
使用ADO连接数据库，同时将数据库中的数据查询显示到界面。
界面显示使用activeX控件datagrid，注意这个文件是datagrid.h/cpp
**********************************************************************************************/




#pragma once
#include "datagrid.h"
#include "DBAdo.h"
#import "C:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

// CDlgSortSystemSQL 对话框

class CDlgSortSystemSQL : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemSQL)

public:
	CDlgSortSystemSQL(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSortSystemSQL();

// 对话框数据
	enum { IDD = IDD_DIALOG_SQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CDBAdo cAdo;
	CDataGrid m_grid;
	void* m_pOwerFrm;				  //回调函数拥有者窗口指针;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton2();
};
