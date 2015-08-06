// DlgSortSystemSQL.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemSQL.h"
#include "afxdialogex.h"
#include "DlgSortSystemOperater.h"


// CDlgSortSystemSQL �Ի���

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


// CDlgSortSystemSQL ��Ϣ�������





BOOL CDlgSortSystemSQL::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	cAdo.CreateInstance();
	cAdo.SetConnectionString();
	if(cAdo.OpenConnection() == false)
		return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgSortSystemSQL::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ASSERT(m_pOwerFrm);
	::PostMessageA( reinterpret_cast<CDlgSortSystemOperater*>(m_pOwerFrm)->m_hWnd, WM_DISPLAYOWERDIALOG, 0, 0 );
	CDialogEx::OnClose();
}


void CDlgSortSystemSQL::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cAdo.OpenRecordset("select * from test");

	m_grid.SetRefDataSource(NULL);
	m_grid.SetRefDataSource((LPUNKNOWN)cAdo.m_ptrRecordset);
	m_grid.Refresh();
	
}




void CDlgSortSystemSQL::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cAdo.OpenRecordset("select * from test2");

	m_grid.SetRefDataSource(NULL);
	m_grid.SetRefDataSource((LPUNKNOWN)cAdo.m_ptrRecordset);
	m_grid.Refresh();
}
