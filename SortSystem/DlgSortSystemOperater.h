#pragma once

#include "MyDib.h"
#include "GrabImage.h"
#include "highgui.h"
#include "ImageProcess.h"
#include "afxwin.h"
#include "OPCAction.h"

// CDlgSortSystemOperater �Ի���

class CDlgSortSystemOperater : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemOperater)

public:
	CDlgSortSystemOperater(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSortSystemOperater();

// �Ի�������
	enum { IDD = IDD_DIALOG_OPERATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void* m_pOwerFrm;				  //�ص�����ӵ���ߴ���ָ��;

	BOOL    InitHCP();


	void    CreateMyDibFromMemory(PBYTE pImage, int nWidth, int nHeight);
	BOOL    RawDataToIplImage(PBYTE pImage, int nWidth, int nHeight );
	void    DrawImage(int nWidth,int nHeight);
	int     ImageDetect();
    static  void WINAPI GrabDisplayCallback(void* pOwner, XRayStatus status, unsigned char* pImgData, long lWidth, long lHeight, XRayPixelFormat pixelFormat);

public:

	sHcpParam*         sHcpParamter; //HCP�����ļ��ṹ��
	sImagePara*        sImageParamter; //Image�����ļ��ṹ��
	vector<KeyPoint> keypoints;

	COPCAction*        m_Opc;
	CGrabImage*        m_Acq;       //
	CMyDib*			   m_pMyDib;	//DIBָ��
	CRITICAL_SECTION   m_csShow; 
	bool               m_bIsGrab;   //�ж��Ƿ��������ɼ���־λ

	int                m_lWidth;
	int                m_lHeight;
	bool               m_bIsManual;

	OPCITEMSTATE       *pValues; //OPC ֵ

	afx_msg void OnBnClickedBtnLoadimage();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnAutodetect();

public:
	CImageProcess*     m_ImgProcess;   //ͼ�����ࡡ�������������
	IplImage*          m_pIplImg;     //��HCP��ȡ����ͼ�� λ��8 �Ҷ�ͼ��

	CListBox           m_listSysMessage;
	void InfoDisplay(char* info);

	afx_msg void OnBnClickedBtnBallin();
	afx_msg void OnBnClickedBtnDetect();
	afx_msg void OnBnClickedBtnBallout();
	afx_msg void OnBnClickedBtnRoll();
	afx_msg void OnBnClickedBtnManualdetect();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnSql();
};
