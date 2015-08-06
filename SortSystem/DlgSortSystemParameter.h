#pragma once
#include "stdafx.h"

// CDlgSortSystemParameter �Ի���

class CDlgSortSystemParameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemParameter)

public:
	CDlgSortSystemParameter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSortSystemParameter();

// �Ի�������
	enum { IDD = IDD_DIALOG_PARAMETER };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButton4(); //�޸�ƽ��̽��������
	afx_msg void OnBnClickedButton3();

	afx_msg void OnBnClickedButton5();
	virtual BOOL OnInitDialog();

public:
    CString m_csFileName;

	//HCP
	CString m_HcpPath;
	int m_iBuf;
	int m_iMode;
	int m_iFrm;
	BOOL m_iGainCorr;
	BOOL m_iOffsetCorr;
	BOOL m_iDftCorr;

		

	//IMAGE
	int m_iRingDeep; //��⹤����������Ƿ��п��� �����
	CString m_TempleteFuleImagePath;   //ȼ����ģ��·��
	CString m_TempleteEmptyImagePath;  //����·��

	
	//OPC 

	void SetHcpControlsFalse();
	void SetHcpControlsTrue();
public:
	void iniHcpConfigRead(CString csFileName);
	void iniHcpConfigWrite(CString csFileName);
 	void HCPVariableToStructMember(sHcpParam *sHcpParamter);

	void iniImageConfigRead(CString csFileName);
	void iniImageConfigWrite(CString csFileName);
	void ImageVariableToStructMember(sImagePara *sImageParamter);

	void SetImageControlsFalse();
	void SetImageControlsTrue();

	void GetFilePathByDlg(CString strMember, int ControlNo);

private:
    BOOL IsHcpControlsChange;
	BOOL IsImageControlsChange;

public:
	afx_msg void OnClose();
	void* m_pOwerFrm;				  //�ص�����ӵ���ߴ���ָ��;

	afx_msg void OnBnClickedBtnImageCancle();
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();

	
};
