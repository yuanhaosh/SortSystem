#pragma once
#include "stdafx.h"

// CDlgSortSystemParameter 对话框

class CDlgSortSystemParameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemParameter)

public:
	CDlgSortSystemParameter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSortSystemParameter();

// 对话框数据
	enum { IDD = IDD_DIALOG_PARAMETER };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButton4(); //修改平板探测器参数
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
	int m_iRingDeep; //检测工件表层下中是否有颗粒 表层厚度
	CString m_TempleteFuleImagePath;   //燃料球模板路径
	CString m_TempleteEmptyImagePath;  //白球路径

	
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
	void* m_pOwerFrm;				  //回调函数拥有者窗口指针;

	afx_msg void OnBnClickedBtnImageCancle();
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();

	
};
