
// SortSystemDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "DlgSortSystemOperater.h"
#include "DlgSortSystemParameter.h"


// CSortSystemDlg �Ի���
class CSortSystemDlg : public CDialogEx
{
// ����
public:
	CSortSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SORTSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
