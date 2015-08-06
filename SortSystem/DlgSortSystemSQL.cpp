// DlgSortSystemSQL.cpp : 实现文件
//

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemSQL.h"
#include "afxdialogex.h"
#include "DlgSortSystemOperater.h"


// CDlgSortSystemSQL 对话框

IMPLEMENT_DYNAMIC(CDlgSortSystemSQL, CDialogEx)

CDlgSortSystemSQL::CDlgSortSystemSQL(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSortSystemSQL::IDD, pParent)
{

}

CDlgSortSystemSQL::~CDlgSortSystemSQL()
{
}

void CDlgSortSystemSQL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATAGRID1, m_grid);
}


BEGIN_MESSAGE_MAP(CDlgSortSystemSQL, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSortSystemSQL::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgSortSystemSQL::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgSortSystemSQL 消息处理程序





BOOL CDlgSortSystemSQL::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	cAdo.CreateInstance();
	cAdo.SetConnectionString();
	if(cAdo.OpenConnection() == false)
		return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSortSystemSQL::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ASSERT(m_pOwerFrm);
	::PostMessageA( reinterpret_cast<CDlgSortSystemOperater*>(m_pOwerFrm)->m_hWnd, WM_DISPLAYOWERDIALOG, 0, 0 );
	CDialogEx::OnClose();
}


void CDlgSortSystemSQL::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	cAdo.OpenRecordset("select * from test");

	m_grid.SetRefDataSource(NULL);
	m_grid.SetRefDataSource((LPUNKNOWN)cAdo.m_ptrRecordset);
	m_grid.Refresh();
	
}




void CDlgSortSystemSQL::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	cAdo.OpenRecordset("select * from test2");

	m_grid.SetRefDataSource(NULL);
	m_grid.SetRefDataSource((LPUNKNOWN)cAdo.m_ptrRecordset);
	m_grid.Refresh();
}
