// DlgSortSystemLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemLoad.h"
#include "afxdialogex.h"


// CDlgSortSystemLoad 对话框

IMPLEMENT_DYNAMIC(CDlgSortSystemLoad, CDialogEx)

CDlgSortSystemLoad::CDlgSortSystemLoad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSortSystemLoad::IDD, pParent)
{

}

CDlgSortSystemLoad::~CDlgSortSystemLoad()
{
}

void CDlgSortSystemLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSortSystemLoad, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CDlgSortSystemLoad::OnBnClickedButtonLoad)
END_MESSAGE_MAP()


// CDlgSortSystemLoad 消息处理程序


//登陆按钮
void CDlgSortSystemLoad::OnBnClickedButtonLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csUserName;
    GetDlgItem(IDC_EDIT_USERNAME)->GetWindowText( csUserName );
}
