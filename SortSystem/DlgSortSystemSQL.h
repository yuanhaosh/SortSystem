/*******************************************************************************************
ʹ��ADO�������ݿ⣬ͬʱ�����ݿ��е����ݲ�ѯ��ʾ�����档
������ʾʹ��activeX�ؼ�datagrid��ע������ļ���datagrid.h/cpp
**********************************************************************************************/




#pragma once
#include "datagrid.h"
#include "DBAdo.h"
#import "C:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

// CDlgSortSystemSQL �Ի���

class CDlgSortSystemSQL : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemSQL)

public:
	CDlgSortSystemSQL(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSortSystemSQL();

// �Ի�������
	enum { IDD = IDD_DIALOG_SQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CDBAdo cAdo;
	CDataGrid m_grid;
	void* m_pOwerFrm;				  //�ص�����ӵ���ߴ���ָ��;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton2();
};
