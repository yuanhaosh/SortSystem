// DlgSortSystemLoad.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemLoad.h"
#include "afxdialogex.h"


// CDlgSortSystemLoad �Ի���

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


// CDlgSortSystemLoad ��Ϣ�������


//��½��ť
void CDlgSortSystemLoad::OnBnClickedButtonLoad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString csUserName;
    GetDlgItem(IDC_EDIT_USERNAME)->GetWindowText( csUserName );
}
