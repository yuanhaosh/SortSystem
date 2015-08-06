// DlgSortSystemOperater.cpp : 实现文件
//
#include <windows.h> 

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemOperater.h"
#include "afxdialogex.h"
#include "SortSystemDlg.h"
#include "ImageProcess.h"
#include "DlgSortSystemSQL.h"


// CDlgSortSystemOperater 对话框

IMPLEMENT_DYNAMIC(CDlgSortSystemOperater, CDialogEx)

CDlgSortSystemOperater::CDlgSortSystemOperater(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSortSystemOperater::IDD, pParent)
{
	m_pOwerFrm = NULL; //调用当前窗口的对话框

	//类
	m_Acq =  NULL;            //平板获取图像类
	m_Opc = NULL;             //opc类
	m_ImgProcess = NULL;      //图像处理类
    m_pMyDib = NULL;          //图像转DIB并且显示类


	//变量
	m_bIsGrab = false;       //执行连续采集
	m_lWidth = 0;            //图像的宽
	m_lHeight = 0;           //图像的高
	m_pIplImg = NULL;        //采集到图像用于图像处理的格式

	m_bIsManual = false;
	

	sHcpParamter = new sHcpParam();          //包含HCP配置文件结构体
	sImageParamter = new sImagePara();
	InitializeCriticalSection(&m_csShow);    //初始临界资源
}

CDlgSortSystemOperater::~CDlgSortSystemOperater()
{}

void CDlgSortSystemOperater::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_listSysMessage);
}

BEGIN_MESSAGE_MAP(CDlgSortSystemOperater, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_LOADIMAGE, &CDlgSortSystemOperater::OnBnClickedBtnLoadimage)
	ON_BN_CLICKED(IDC_BTN_SNAP, &CDlgSortSystemOperater::OnBnClickedBtnSnap)
	ON_BN_CLICKED(IDC_BTN_AUTODETECT, &CDlgSortSystemOperater::OnBnClickedBtnAutodetect)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_BALLIN, &CDlgSortSystemOperater::OnBnClickedBtnBallin)
	ON_BN_CLICKED(IDC_BTN_DETECT, &CDlgSortSystemOperater::OnBnClickedBtnDetect)
	ON_BN_CLICKED(IDC_BTN_BALLOUT, &CDlgSortSystemOperater::OnBnClickedBtnBallout)
	ON_BN_CLICKED(IDC_BTN_ROLL, &CDlgSortSystemOperater::OnBnClickedBtnRoll)
	ON_BN_CLICKED(IDC_BTN_MANUALDETECT, &CDlgSortSystemOperater::OnBnClickedBtnManualdetect)
	ON_BN_CLICKED(IDC_BTN_RESET, &CDlgSortSystemOperater::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_SQL, &CDlgSortSystemOperater::OnBnClickedBtnSql)
END_MESSAGE_MAP()


// CDlgSortSystemOperater 消息处理程序

void CDlgSortSystemOperater::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ASSERT(m_pOwerFrm);
	::PostMessageA( reinterpret_cast<CSortSystemDlg*>(m_pOwerFrm)->m_hWnd, WM_DISPLAYOWERDIALOG, 0, 0 );
	CDialogEx::OnClose();
}

BOOL CDlgSortSystemOperater::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//1. 读取配置文件，获取配置参数
	CDlgSortSystemParameter CDlgParam = new CDlgSortSystemParameter();
	CDlgParam.HCPVariableToStructMember(sHcpParamter);
	CDlgParam.ImageVariableToStructMember(sImageParamter);

	//return TRUE;
	//2. 初始化探测器
	InitHCP();

	//3. 初始化OPC Sserver
	if(m_Opc == NULL)
	{
		m_Opc = new COPCAction();
	}
	if(!m_Opc->InitOPCServer())
	{
		AfxMessageBox("OPC初始化失败！");
		delete m_Opc;
		return FALSE;
	}

	//4. PLC自检
	int result = m_Opc->CheckPLCStateM0(0);
	if(result != 0)
	{
		InfoDisplay("PLC启动自检异常！");
		GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false);
	    GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(false);

		result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[8],  0xFF);  //VB6 FF
		if(result == FALSE)
		{
			AfxMessageBox("VB6置位异常！"); 
		    return FALSE;
		}

	}
	

	GetDlgItem(IDC_BTN_BALLIN)->EnableWindow(false); 
	GetDlgItem(IDC_BTN_DETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_BALLOUT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_ROLL)->EnableWindow(false);
	SetTimer(1, 1000, NULL);   
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgSortSystemOperater::InitHCP()
{
	if(m_Acq == NULL)
	{
		m_Acq = new CGrabImage();
	}
	m_Acq->SetOwner(this, GrabDisplayCallback);
	if (!m_Acq->InitHCPFromIndex(sHcpParamter))
    {
		InfoDisplay("平板探测器初始化失败！");
		m_Acq->Destroy(); 
		delete m_Acq;
		m_Acq = NULL;

		GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false); //平板探测器连接失败，无法进行自动检测和手动检测
	    GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(false);
		GetDlgItem(IDC_BTN_SNAP)->EnableWindow(false);
		GetDlgItem(IDC_BTN_RESET)->EnableWindow(false);

		GetDlgItem(IDC_BTN_BALLIN)->EnableWindow(false);
		GetDlgItem(IDC_BTN_DETECT)->EnableWindow(false);
		GetDlgItem(IDC_BTN_BALLOUT)->EnableWindow(false);
		GetDlgItem(IDC_BTN_ROLL)->EnableWindow(false);
		
		return FALSE;
    }
	return TRUE;
}

void CDlgSortSystemOperater::GrabDisplayCallback(void* pOwner,XRayStatus status, unsigned char* pImgData, long lWidth, long lHeight, XRayPixelFormat pixelFormat)
{
	CDlgSortSystemOperater *pDlg = (CDlgSortSystemOperater*)pOwner;
    
	if (!pImgData) return;	

	EnterCriticalSection(&pDlg->m_csShow);
	pDlg->m_lWidth=lWidth;
	pDlg->m_lHeight=lHeight;
	pDlg->CreateMyDibFromMemory(pImgData,lWidth, lHeight);
    pDlg->PostMessage(WM_DISPLAYMESSAGE,0,0);
	LeaveCriticalSection(&pDlg->m_csShow); 
}

//用图像原始数据生成m_pMyDib对象和IplImage对象
void CDlgSortSystemOperater::CreateMyDibFromMemory(PBYTE pImage, int nWidth, int nHeight)
{
	if(m_pMyDib!=NULL)
	{
		 delete m_pMyDib;
		m_pMyDib = NULL;
	}
	m_pMyDib = new CMyDib(nWidth, nHeight, CMyDib::FormatUByte, pImage, TRUE);

	RawDataToIplImage(pImage, nWidth, nHeight);
	ImageDetect();
}

//探测器采集到的数据转成用于可被OPENCV处理的图像格式
BOOL CDlgSortSystemOperater::RawDataToIplImage(PBYTE pImage, int nWidth, int nHeight )
{
	m_pIplImg = cvCreateImageHeader(cvSize(nWidth, nHeight),  IPL_DEPTH_8U , 1);
    cvSetData(m_pIplImg, pImage, nWidth); //img->imageData  = 你的帧内存数据;
	return TRUE;
}

// not used
void CDlgSortSystemOperater::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	
}

//显示在对话框
void CDlgSortSystemOperater::DrawImage(int nWidth,int nHeight)
{
	CRect rcDIB;
	CRect rcDC;
	rcDIB = CRect(0, 0, nWidth, nHeight);
	rcDIB.OffsetRect(0, 0);
	
	CDC* pDC = this->GetDlgItem(IDC_PIC)->GetDC();
	HDC hDispHdc = pDC->GetSafeHdc();
	pDC->GetWindow()->GetClientRect(rcDC);

	m_pMyDib->Paint(hDispHdc, rcDC, rcDIB); //控件右344 

	/*标记斑点 */
	// 创建画笔
	CPen *pPenRed = new CPen; 
	pPenRed->CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); //设置画笔参数
	CGdiObject* pOldPen = pDC->SelectObject(pPenRed);
	pDC->SelectStockObject(NULL_BRUSH); // 空心圆，不加为实心圆
	for(int i = 0; i < keypoints.size(); i++)
	{
		pDC->Ellipse( keypoints[i].pt.x - keypoints[i].size,  
			nHeight/2 - keypoints[i].pt.y - keypoints[i].size, 
			          keypoints[i].pt.x + keypoints[i].size,
			nHeight/2 - keypoints[i].pt.y + keypoints[i].size ); //画图坐标和图像坐标是垂直互补 344 318
	}

	/// 恢复以前的画笔 
	delete pPenRed;
	pDC->SelectObject(pOldPen); 
	// ================================添加画园代码结束
	ReleaseDC(pDC);
}

//加载本地文件
void CDlgSortSystemOperater::OnBnClickedBtnLoadimage()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pMyDib)
	{
		delete m_pMyDib;
        m_pMyDib=NULL;

		m_lWidth = 0;
		m_lHeight = 0;
	}
	m_pMyDib=new CMyDib();

	static char BASED_CODE file[] = ("BMP Files (*.bmp) |*.bmp|all type (*.*)|*.*||");//过滤器
	CFileDialog  DlgFile(TRUE);//, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, file, NULL);
	CString m_strPath;
	if(DlgFile.DoModal()==IDOK)
	{
		m_strPath=(LPCTSTR)DlgFile.GetPathName();
	}
	else
	{
		return;
	}
	
	//效率测试1
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;

	
	//效率测试2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	m_pMyDib->ReadBMPFile(m_strPath);

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();
	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);

	//效率测试3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	char str[100] = {0};
	sprintf_s(str, sizeof(str),"图像预处理耗时%f 秒", time);
	InfoDisplay(str);


	//效率测试2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);

	ImageDetect();

	//效率测试3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	//str[100] = {0};
	memset(str, 0, sizeof(str));
	sprintf_s(str, sizeof(str),"图像处理耗时%f 秒", time);
	InfoDisplay(str);

	//效率测试2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);

	DrawImage(m_lWidth,m_lHeight);

	//效率测试3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	char str1[100] = {0};
	sprintf_s(str1, sizeof(str1),"显示图像耗时%f 秒", time);
	InfoDisplay(str1);
}

//
LRESULT CDlgSortSystemOperater::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
		switch(message)
	{
		  case WM_DISPLAYMESSAGE:
              EnterCriticalSection(&m_csShow);
			  //图像检测
			  ImageDetect();
			  //图像显示
			  DrawImage(m_lWidth,m_lHeight);
			  LeaveCriticalSection(&m_csShow);
			  break;
		    case WM_DISPLAYOWERDIALOG:
              ShowWindow(SW_SHOWNORMAL);
			  break;
		  default:
			  break;
	  }

	return CDialogEx::DefWindowProc(message, wParam, lParam);
}

//单帧采集
void CDlgSortSystemOperater::OnBnClickedBtnSnap()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(true);

	//PLC 检测M0.4
	int result = -1;
	if(!m_Opc->IsOPCOnline()) //OPC 连接性检查
	{
		m_Opc->InitOPCServer();
	}	
	while(1)
	{
		if(m_Opc->CheckPLCStateM0(4) != 1)  //测试位有球
		{
			if(m_Opc->CheckPLCStateM0(2) == 1) //进球位无球
			{
				InfoDisplay("进球位无球。"); //PLC 警告窗提示M0.2
			}
			else
			{
				result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0 进球
				if(result == FALSE)
					continue;
			}
		}
		else
		{
			memset(pValues, 0, sizeof(OPCITEMSTATE));
			result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[3], OPC_DS_CACHE);  //VB1 进球到位
			if( pValues->vDataValue.bVal  == 0XFF)
				break;
		}
	}

	if (m_Acq == NULL && !m_Acq->IsHCPOnline())
	{
		InfoDisplay("平板探测器连接中断，即将重新连接。");
		if(!m_Acq->IsHCPOnline())
		{
			m_Acq->InitHCP(sHcpParamter);
		}
		else 
		{
			m_Acq = new CGrabImage();
			m_Acq->SetOwner(this, GrabDisplayCallback);
			if (!m_Acq->InitHCPFromIndex(sHcpParamter))
			{
				AfxMessageBox("平板探测器初始化失败！");
				InfoDisplay("平板探测器初始化失败！");
				m_Acq->Destroy(); 
				delete m_Acq;
				m_Acq = NULL;
				return ;
			}
		}
	}
	
	if (m_Acq->IsGrabbing())
	{
		m_Acq->Freeze(); //终止连续采集
	}

	if(!m_Acq->SnapImage())
	{
	    AfxMessageBox("采集图像失败!");	
	}

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();

	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
	ImageDetect();
}

//自动检测
void CDlgSortSystemOperater::OnBnClickedBtnAutodetect()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(true);
	
	int result = -1;
	OPCITEMSTATE *pValues = new OPCITEMSTATE[1];
	
	while(1)
	{
		while(1) //出球结束
		{
			memset(pValues, 0, sizeof(OPCITEMSTATE));
			result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[6], OPC_DS_CACHE);  //VB4
			if( pValues->vDataValue.bVal  == 0XFF)
				break;
			Sleep(100);
		}

		while(1) 
		{
			if(m_Opc->CheckPLCStateM0(4) != 1)  //测试位有球
			{
				if(m_Opc->CheckPLCStateM0(2) == 1) //进球位无球
				{
					InfoDisplay("进球位无球。"); //PLC 警告窗提示M0.2
				}
				else
				{
					result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0 进球
					if(result == FALSE)
						continue;
				}
			}
			else
			{
				memset(pValues, 0, sizeof(OPCITEMSTATE));
				result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[3], OPC_DS_CACHE);  //VB1 进球到位
				if( pValues->vDataValue.bVal  == 0XFF)
					break;
			}
		}

		if (m_Acq->IsGrabbing())
		{
			m_Acq->Freeze(); 
		}
		if (!m_Acq->SnapImage()) 
		{
			AfxMessageBox("采集图像失败");
		}
		m_lWidth = m_pMyDib->Width();
		m_lHeight = m_pMyDib->Height();

		RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
		ImageDetect();

	}
	delete[] pValues;
}

//图像识别
int CDlgSortSystemOperater::ImageDetect()
{
	keypoints.clear();

	int result = 0;
	if(m_ImgProcess == NULL)
	{
		m_ImgProcess = new CImageProcess(sImageParamter);
	}
	m_ImgProcess->ReadTemplateImage();
	m_ImgProcess->ReadChkImage(m_pIplImg);
	
	int ret = m_ImgProcess->ClassifyImage();
	if(ret == -1)
	{
		//输出提示
		InfoDisplay("检测失败！");
	}
	else if(ret == 0) //程序异常
	{

	}
	else if(ret == 1) //燃料球 合格的
	{
		InfoDisplay("合格燃料球");
		if(m_Opc != NULL && m_Opc->IsOPCOnline())
	    {
			result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[4], 0XFF);  //VB2
			if(result == FALSE)
			{
				return -1;
			}
	    }
				
	}
	else if(ret == 2) //燃料球 不合格
	{
		keypoints = m_ImgProcess->GetKeypoints();
		InfoDisplay("不合格燃料球");
		if(m_Opc != NULL && m_Opc->IsOPCOnline())
		{
			result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[5], 0XFF);  //VB3
			if(result == FALSE)
			{
				return -1;
			}
		}
	}
	else if(ret == 3) //检测球
	{
		InfoDisplay("检测球");
	}
	else if(ret = 4) //白球
	{
		InfoDisplay("白球");
	}

	return 0;
}

//定时器 检查plc状态
void CDlgSortSystemOperater::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int result = 0;
	switch(nIDEvent)
	{
		case 1: if(m_Opc->CheckPLCStateM0(0) == 1)
				{
					m_Opc->ShowPLCStateM1(); 
					KillTimer(1); 
				}
			break;
		default: break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


//手动检测 切换按钮
void CDlgSortSystemOperater::OnBnClickedBtnManualdetect()
{
	// TODO: 在此添加控件通知处理程序代码

	if(m_bIsManual == false) //
	{  
		GetDlgItem(IDC_BTN_BALLIN)->EnableWindow(true);
		GetDlgItem(IDC_BTN_DETECT)->EnableWindow(true);
		GetDlgItem(IDC_BTN_BALLOUT)->EnableWindow(true);
		GetDlgItem(IDC_BTN_ROLL)->EnableWindow(true);

		m_bIsManual = true;
	}
	else
	{
		GetDlgItem(IDC_BTN_BALLIN)->EnableWindow(false);
		GetDlgItem(IDC_BTN_DETECT)->EnableWindow(false);
		GetDlgItem(IDC_BTN_BALLOUT)->EnableWindow(false);
		GetDlgItem(IDC_BTN_ROLL)->EnableWindow(false);

		m_bIsManual = false;
	}
}
//进球 1 //单步执行 共4步
void CDlgSortSystemOperater::OnBnClickedBtnBallin()
{
	// TODO: 在此添加控件通知处理程序代码
	int result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0
	if(result == FALSE)
	{
		return ;
	}

}
//识别 2
void CDlgSortSystemOperater::OnBnClickedBtnDetect()
{
	// TODO: 在此添加控件通知处理程序代码

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();

	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
	ImageDetect();
}
//转动 3
void CDlgSortSystemOperater::OnBnClickedBtnRoll()
{
	// TODO: 在此添加控件通知处理程序代码
}
//出球 4
void CDlgSortSystemOperater::OnBnClickedBtnBallout()
{
	// TODO: 在此添加控件通知处理程序代码
}



//复位
void CDlgSortSystemOperater::OnBnClickedBtnReset()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxMessageBox("系统异常，使用复位键重置");
	int result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[7], 0XFF);  //VB5
	if(result == FALSE)
	{
		return ;
	}
}

//listbox 显示提示信息
void CDlgSortSystemOperater::InfoDisplay(char* info)
{
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
	char infoHeadTime[MAX_STR];
    sprintf_s(infoHeadTime, sizeof(infoHeadTime),"%02d:%02d:%02d ", sys.wHour, sys.wMinute, sys.wSecond);
	strcat_s(infoHeadTime,sizeof(infoHeadTime), info);
	m_listSysMessage.AddString(infoHeadTime );
}


void CDlgSortSystemOperater::OnBnClickedBtnSql()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgSortSystemSQL CDlgParam;
	CDlgParam.m_pOwerFrm = reinterpret_cast<void *>(this);
	ShowWindow(SW_HIDE);
	CDlgParam.DoModal();
}
