#pragma once

#include "MyDib.h"
#include "GrabImage.h"
#include "highgui.h"
#include "ImageProcess.h"
#include "afxwin.h"
#include "OPCAction.h"

// CDlgSortSystemOperater 对话框

class CDlgSortSystemOperater : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortSystemOperater)

public:
	CDlgSortSystemOperater(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSortSystemOperater();

// 对话框数据
	enum { IDD = IDD_DIALOG_OPERATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void* m_pOwerFrm;				  //回调函数拥有者窗口指针;

	BOOL    InitHCP();


	void    CreateMyDibFromMemory(PBYTE pImage, int nWidth, int nHeight);
	BOOL    RawDataToIplImage(PBYTE pImage, int nWidth, int nHeight );
	void    DrawImage(int nWidth,int nHeight);
	int     ImageDetect();
    static  void WINAPI GrabDisplayCallback(void* pOwner, XRayStatus status, unsigned char* pImgData, long lWidth, long lHeight, XRayPixelFormat pixelFormat);

public:

	sHcpParam*         sHcpParamter; //HCP配置文件结构体
	sImagePara*        sImageParamter; //Image配置文件结构体
	vector<KeyPoint> keypoints;

	COPCAction*        m_Opc;
	CGrabImage*        m_Acq;       //
	CMyDib*			   m_pMyDib;	//DIB指针
	CRITICAL_SECTION   m_csShow; 
	bool               m_bIsGrab;   //判断是否在连续采集标志位

	int                m_lWidth;
	int                m_lHeight;
	bool               m_bIsManual;

	OPCITEMSTATE       *pValues; //OPC 值

	afx_msg void OnBnClickedBtnLoadimage();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnAutodetect();

public:
	CImageProcess*     m_ImgProcess;   //图像处理类　包含ｏｐｅｎｃｖ库
	IplImage*          m_pIplImg;     //从HCP获取到的图像 位深8 灰度图像

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
