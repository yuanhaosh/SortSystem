#pragma once


// CDlgSortSystemLoad �Ի���

class CDlgSortSystemLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemLoad)

public:
	CDlgSortSystemLoad(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSortSystemLoad();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad();
};
