
// SortSystemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SortSystem.h"
#include "SortSystemDlg.h"
#include "afxdialogex.h"
#include "DlgSortSystemParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSortSystemDlg 对话框




CSortSystemDlg::CSortSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSortSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSortSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TAB1, m_Tab);
}

BEGIN_MESSAGE_MAP(CSortSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CSortSystemDlg::OnBnClickedButton1)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSortSystemDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PARAM, &CSortSystemDlg::OnClickedButtonParam)
	ON_BN_CLICKED(IDC_BUTTON_OPERATE, &CSortSystemDlg::OnBnClickedButtonOperate)
END_MESSAGE_MAP()


// CSortSystemDlg 消息处理程序

BOOL CSortSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOWNORMAL);

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSortSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSortSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSortSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSortSystemDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	

}



void CSortSystemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

//调出参数界面
void CSortSystemDlg::OnClickedButtonParam()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgSortSystemParameter CDlgParam;
	CDlgParam.m_pOwerFrm = reinterpret_cast<void *>(this);
	ShowWindow(SW_HIDE);
	CDlgParam.DoModal();

	
}

//调出日常界面
void CSortSystemDlg::OnBnClickedButtonOperate()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgSortSystemOperater CDlgOperate;
	CDlgOperate.m_pOwerFrm = reinterpret_cast<void *>(this);
	ShowWindow(SW_HIDE);
	CDlgOperate.DoModal();
}



LRESULT CSortSystemDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
		  case WM_DISPLAYOWERDIALOG:
              ShowWindow(SW_SHOWNORMAL);
			  break;
		  default:
			  break;
	  }
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}
