// DlgSortSystemParameter.cpp : 实现文件
//

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemParameter.h"
#include "afxdialogex.h"
#include "SortSystemDlg.h"

// CDlgSortSystemParameter 对话框

IMPLEMENT_DYNAMIC(CDlgSortSystemParameter, CDialogEx)

CDlgSortSystemParameter::CDlgSortSystemParameter(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSortSystemParameter::IDD, pParent)
	, m_HcpPath(_T(""))
	, m_iBuf(0)
	, m_iMode(0)
	, m_iFrm(0)
	, m_iGainCorr(FALSE)
	, m_iOffsetCorr(FALSE)
	, m_iDftCorr(FALSE)
	, m_iRingDeep(0)
	, m_TempleteFuleImagePath(_T(""))
	, m_TempleteEmptyImagePath(_T(""))
{
	
	m_pOwerFrm = NULL;
}

CDlgSortSystemParameter::~CDlgSortSystemParameter()
{
}

void CDlgSortSystemParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RECEPTOR_PATH, m_HcpPath);
	DDX_Text(pDX, IDC_EDIT_BUF, m_iBuf);
	DDX_Text(pDX, IDC_EDIT_MODE, m_iMode);
	DDX_Text(pDX, IDC_EDIT_FRM, m_iFrm);
	DDX_Check(pDX, IDC_CHECK_GAIN_CORRECTION, m_iGainCorr);
	DDX_Check(pDX, IDC_CHECK_OFFSET_CORRECTION, m_iOffsetCorr);
	DDX_Check(pDX, IDC_CHECK_DFT_CORRECTION, m_iDftCorr);
	DDX_Text(pDX, IDC_EDIT_RING_WIDTH, m_iRingDeep);
	DDX_Text(pDX, IDC_EDIT_FULE_PATH, m_TempleteFuleImagePath);
	DDX_Text(pDX, IDC_EDIT_EMPTY_PATH, m_TempleteEmptyImagePath);
}


BEGIN_MESSAGE_MAP(CDlgSortSystemParameter, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgSortSystemParameter::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgSortSystemParameter::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgSortSystemParameter::OnBnClickedButton5)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_IMAGE_CANCLE, &CDlgSortSystemParameter::OnBnClickedBtnImageCancle)
	ON_BN_CLICKED(IDC_BTN_IMAGE, &CDlgSortSystemParameter::OnBnClickedBtnImage)
	ON_BN_CLICKED(IDC_BUTTON6, &CDlgSortSystemParameter::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CDlgSortSystemParameter::OnBnClickedButton7)
END_MESSAGE_MAP()


// CDlgSortSystemParameter 消息处理程序

BOOL CDlgSortSystemParameter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//读取所有参数
	iniHcpConfigRead(m_csFileName);
	UpdateData(FALSE);

	IsHcpControlsChange = FALSE;
	GetDlgItem(IDC_BUTTON4)->SetWindowTextA("激活修改");
	SetHcpControlsFalse();
	
	iniImageConfigRead(m_csFileName);
	UpdateData(FALSE);
	IsImageControlsChange = FALSE;
	GetDlgItem(IDC_BTN_IMAGE)->SetWindowTextA("激活修改");
	SetImageControlsFalse();



	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//HCP读写配置文件到关联变量
void CDlgSortSystemParameter::iniHcpConfigRead(CString csFileName = NULL)
{
	// TODO: Add your command handler code here
	//读入字符串
	if(csFileName.IsEmpty())
	{
		csFileName = CONFIGFILENAME;//"./config.ini";
	}
	GetPrivateProfileString("HCP", _T("平板探测器路径"), NULL, m_HcpPath.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 
	//读入整型值 
	m_iBuf  = GetPrivateProfileInt("HCP", "BUF", -1, csFileName); 
	m_iMode = GetPrivateProfileInt("HCP", "MODE",-1, csFileName); 
	m_iFrm  = GetPrivateProfileInt("HCP", "FRM", -1, csFileName); 

	CString csTemp;
	GetPrivateProfileString("HCP", _T("增益校正"), NULL, csTemp.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 
	m_iGainCorr = csTemp.Compare("TRUE") == 0 ? TRUE : FALSE ;

	GetPrivateProfileString("HCP", _T("偏置校正"), NULL, csTemp.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 
	m_iOffsetCorr = csTemp.Compare("TRUE") == 0 ? TRUE : FALSE ;

	GetPrivateProfileString("HCP", _T("坏像素校正"), NULL, csTemp.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 
	m_iDftCorr = csTemp.Compare("TRUE") == 0 ? TRUE : FALSE ;

}
void CDlgSortSystemParameter::iniHcpConfigWrite(CString csFileName = NULL) 
{
	// TODO: Add your command handler code here

	UpdateData(TRUE); //控件到变量的值刷新

	if(csFileName.IsEmpty())
	{
		csFileName = CONFIGFILENAME;// "./config.ini";
	}
	CString strTemp;
	WritePrivateProfileString("HCP", _T("平板探测器路径"), m_HcpPath, csFileName); 
	
	strTemp.Format("%d", m_iBuf); 
	WritePrivateProfileString("HCP", "BUF", strTemp, csFileName);

	strTemp.Format("%d", m_iMode); 
	WritePrivateProfileString("HCP", "MODE", strTemp, csFileName);

	strTemp.Format("%d", m_iFrm); 
	WritePrivateProfileString("HCP", "FRM", strTemp, csFileName);

	strTemp = m_iGainCorr == TRUE ? "TRUE" : "FALSE";
	WritePrivateProfileString("HCP", _T("增益校正"), strTemp, csFileName);

	strTemp = m_iOffsetCorr == TRUE ? "TRUE" : "FALSE";
	WritePrivateProfileString("HCP", _T("偏置校正"), strTemp, csFileName);

    strTemp = m_iDftCorr == TRUE ? "TRUE" : "FALSE";
	WritePrivateProfileString("HCP", _T("坏像素校正"), strTemp, csFileName);
}

//图像处理读写配置文件
void CDlgSortSystemParameter::iniImageConfigRead(CString csFileName = NULL)
{
	//UpdateData(TRUE); //控件到变量的值刷新

	if(csFileName.IsEmpty())
	{
		csFileName = CONFIGFILENAME;// "./config.ini";
	}
	m_iRingDeep  = GetPrivateProfileInt("IMAGE", "RINGDEEP", -1, csFileName); 

	GetPrivateProfileString("IMAGE", _T("燃料球模板路径"), NULL, m_TempleteFuleImagePath.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 
	GetPrivateProfileString("IMAGE", _T("白球模板路径"),   NULL, m_TempleteEmptyImagePath.GetBuffer(MAX_PATH),MAX_PATH, csFileName); 

}
void CDlgSortSystemParameter::iniImageConfigWrite(CString csFileName = NULL)
{
	UpdateData(TRUE); //控件到变量的值刷新

	if(csFileName.IsEmpty())
	{
		csFileName = CONFIGFILENAME;// "./config.ini";
	}
	CString strTemp;
	strTemp.Format("%d", m_iRingDeep); 
	WritePrivateProfileString("IMAGE", "RINGDEEP", strTemp, csFileName);

	WritePrivateProfileString("IMAGE", _T("燃料球模板路径"), m_TempleteFuleImagePath,  csFileName); 
	WritePrivateProfileString("IMAGE", _T("白球模板路径"),   m_TempleteEmptyImagePath, csFileName); 
}

//关连变量值转到结构体里
void CDlgSortSystemParameter::HCPVariableToStructMember(sHcpParam* sHcpParamter)
{
	iniHcpConfigRead();
	//sHcpParamter->HcpPath = {0};
	strcpy(sHcpParamter->HcpPath, m_HcpPath.GetBuffer(0));//, m_HcpPath.GetLength()+1);

	//sHcpParamter->HcpPath = m_HcpPath;

	sHcpParamter->Buf = m_iBuf;
	sHcpParamter->Mode = m_iMode;
	sHcpParamter->Frm = m_iFrm;
	sHcpParamter->GainCorr = m_iGainCorr;
	sHcpParamter->OffsetCorr = m_iOffsetCorr;
	sHcpParamter->DftCorr = m_iDftCorr;
	
}
void CDlgSortSystemParameter::ImageVariableToStructMember(sImagePara *sImageParamter)
{
	iniImageConfigRead();
	sImageParamter->RingDeep = m_iRingDeep;
	strcpy(sImageParamter->FulePath, m_TempleteFuleImagePath.GetBuffer(0));
	strcpy(sImageParamter->EmptyPath, m_TempleteEmptyImagePath.GetBuffer(0));
}



//修改平板探测器参数 //取消修改平板探测器参数
void CDlgSortSystemParameter::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if(IsHcpControlsChange == FALSE)
	{
		GetDlgItem(IDC_BUTTON4)->SetWindowTextA("修改");
		SetHcpControlsTrue();
		IsHcpControlsChange = TRUE;
	}
	else if(IsHcpControlsChange == TRUE)
	{
		UpdateData(TRUE);
	    iniHcpConfigWrite();
		OnBnClickedButton5();
		
	}
}
void CDlgSortSystemParameter::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON4)->SetWindowTextA("激活修改");
	UpdateData(FALSE);
	SetHcpControlsFalse();
	IsHcpControlsChange = FALSE;
}

//相关控件置灰及恢复
void CDlgSortSystemParameter::SetHcpControlsFalse()
{
	GetDlgItem(IDC_EDIT_RECEPTOR_PATH)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_BUF)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_MODE)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_FRM)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_GAIN_CORRECTION)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_OFFSET_CORRECTION)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_DFT_CORRECTION)->EnableWindow(false);
}
void CDlgSortSystemParameter::SetHcpControlsTrue()
{
	GetDlgItem(IDC_EDIT_RECEPTOR_PATH)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_BUF)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_MODE)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_FRM)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_GAIN_CORRECTION)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_OFFSET_CORRECTION)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_DFT_CORRECTION)->EnableWindow(true);

}
void CDlgSortSystemParameter::SetImageControlsFalse()
{
	GetDlgItem(IDC_EDIT_RING_WIDTH)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_FULE_PATH)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_EMPTY_PATH)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(false);
}
void CDlgSortSystemParameter::SetImageControlsTrue()
{
	GetDlgItem(IDC_EDIT_RING_WIDTH)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_FULE_PATH)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_EMPTY_PATH)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(true);
}


void CDlgSortSystemParameter::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ASSERT(m_pOwerFrm);
	::PostMessageA( reinterpret_cast<CSortSystemDlg*>(m_pOwerFrm)->m_hWnd, WM_DISPLAYOWERDIALOG, 0, 0 );
	CDialogEx::OnClose();
	
}

//修改Image参数 //取消修改Image参数
void CDlgSortSystemParameter::OnBnClickedBtnImage()
{
	// TODO: 在此添加控件通知处理程序代码
	if(IsImageControlsChange == FALSE)
	{
		GetDlgItem(IDC_BTN_IMAGE)->SetWindowTextA("修改");
		SetImageControlsTrue();
		IsImageControlsChange = TRUE;
	}
	else if(IsImageControlsChange == TRUE)
	{
		UpdateData(TRUE);
	    iniImageConfigWrite();
		OnBnClickedBtnImageCancle();
		
	}
}
void CDlgSortSystemParameter::OnBnClickedBtnImageCancle()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_IMAGE)->SetWindowTextA("激活修改");
	UpdateData(FALSE);
	SetImageControlsFalse();
	IsImageControlsChange = FALSE;
}



//选择平板探测器路径
void CDlgSortSystemParameter::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	GetFilePathByDlg(m_HcpPath, IDC_EDIT_RECEPTOR_PATH);

}
//选择燃料球路径
void CDlgSortSystemParameter::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(1,NULL,NULL,OFN_HIDEREADONLY ,"All Files(*.*)|*.*||");
	if(IDOK!=dlg.DoModal())
		return;
	m_TempleteFuleImagePath = dlg.GetPathName();

	GetDlgItem(IDC_EDIT_FULE_PATH)->SetWindowText(m_TempleteFuleImagePath);
}
//选择白球路径
void CDlgSortSystemParameter::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(1,NULL,NULL,OFN_HIDEREADONLY ,"All Files(*.*)|*.*||");
	if(IDOK!=dlg.DoModal())
		return;
	m_TempleteEmptyImagePath = dlg.GetPathName();

	GetDlgItem(IDC_EDIT_EMPTY_PATH)->SetWindowText(m_TempleteEmptyImagePath);
}
//调用对话框选择文件路径
void CDlgSortSystemParameter::GetFilePathByDlg(CString strMember, int CtrlNo)
{
	CString str;

	BROWSEINFO stInfo = {NULL};
	LPCITEMIDLIST pIdlst;
	TCHAR szPath[MAX_PATH];
	stInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS; //| BIF_USENEWUI;
	stInfo.lpszTitle= "请选择路径:";
	pIdlst = SHBrowseForFolder(&stInfo);
	if(!pIdlst) return ;
	if(!SHGetPathFromIDList(pIdlst, szPath)) return ;
	
	strMember = szPath;
	GetDlgItem(CtrlNo)->SetWindowText(szPath);
    
}
