// DlgSortSystemOperater.cpp : ʵ���ļ�
//
#include <windows.h> 

#include "stdafx.h"
#include "SortSystem.h"
#include "DlgSortSystemOperater.h"
#include "afxdialogex.h"
#include "SortSystemDlg.h"
#include "ImageProcess.h"
#include "DlgSortSystemSQL.h"


// CDlgSortSystemOperater �Ի���

IMPLEMENT_DYNAMIC(CDlgSortSystemOperater, CDialogEx)

CDlgSortSystemOperater::CDlgSortSystemOperater(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSortSystemOperater::IDD, pParent)
{
	m_pOwerFrm = NULL; //���õ�ǰ���ڵĶԻ���

	//��
	m_Acq =  NULL;            //ƽ���ȡͼ����
	m_Opc = NULL;             //opc��
	m_ImgProcess = NULL;      //ͼ������
    m_pMyDib = NULL;          //ͼ��תDIB������ʾ��


	//����
	m_bIsGrab = false;       //ִ�������ɼ�
	m_lWidth = 0;            //ͼ��Ŀ�
	m_lHeight = 0;           //ͼ��ĸ�
	m_pIplImg = NULL;        //�ɼ���ͼ������ͼ����ĸ�ʽ

	m_bIsManual = false;
	

	sHcpParamter = new sHcpParam();          //����HCP�����ļ��ṹ��
	sImageParamter = new sImagePara();
	InitializeCriticalSection(&m_csShow);    //��ʼ�ٽ���Դ
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


// CDlgSortSystemOperater ��Ϣ�������

void CDlgSortSystemOperater::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ASSERT(m_pOwerFrm);
	::PostMessageA( reinterpret_cast<CSortSystemDlg*>(m_pOwerFrm)->m_hWnd, WM_DISPLAYOWERDIALOG, 0, 0 );
	CDialogEx::OnClose();
}

BOOL CDlgSortSystemOperater::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//1. ��ȡ�����ļ�����ȡ���ò���
	CDlgSortSystemParameter CDlgParam = new CDlgSortSystemParameter();
	CDlgParam.HCPVariableToStructMember(sHcpParamter);
	CDlgParam.ImageVariableToStructMember(sImageParamter);

	//return TRUE;
	//2. ��ʼ��̽����
	InitHCP();

	//3. ��ʼ��OPC Sserver
	if(m_Opc == NULL)
	{
		m_Opc = new COPCAction();
	}
	if(!m_Opc->InitOPCServer())
	{
		AfxMessageBox("OPC��ʼ��ʧ�ܣ�");
		delete m_Opc;
		return FALSE;
	}

	//4. PLC�Լ�
	int result = m_Opc->CheckPLCStateM0(0);
	if(result != 0)
	{
		InfoDisplay("PLC�����Լ��쳣��");
		GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false);
	    GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(false);

		result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[8],  0xFF);  //VB6 FF
		if(result == FALSE)
		{
			AfxMessageBox("VB6��λ�쳣��"); 
		    return FALSE;
		}

	}
	

	GetDlgItem(IDC_BTN_BALLIN)->EnableWindow(false); 
	GetDlgItem(IDC_BTN_DETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_BALLOUT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_ROLL)->EnableWindow(false);
	SetTimer(1, 1000, NULL);   
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
		InfoDisplay("ƽ��̽������ʼ��ʧ�ܣ�");
		m_Acq->Destroy(); 
		delete m_Acq;
		m_Acq = NULL;

		GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false); //ƽ��̽��������ʧ�ܣ��޷������Զ������ֶ����
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

//��ͼ��ԭʼ��������m_pMyDib�����IplImage����
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

//̽�����ɼ���������ת�����ڿɱ�OPENCV�����ͼ���ʽ
BOOL CDlgSortSystemOperater::RawDataToIplImage(PBYTE pImage, int nWidth, int nHeight )
{
	m_pIplImg = cvCreateImageHeader(cvSize(nWidth, nHeight),  IPL_DEPTH_8U , 1);
    cvSetData(m_pIplImg, pImage, nWidth); //img->imageData  = ���֡�ڴ�����;
	return TRUE;
}

// not used
void CDlgSortSystemOperater::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	
}

//��ʾ�ڶԻ���
void CDlgSortSystemOperater::DrawImage(int nWidth,int nHeight)
{
	CRect rcDIB;
	CRect rcDC;
	rcDIB = CRect(0, 0, nWidth, nHeight);
	rcDIB.OffsetRect(0, 0);
	
	CDC* pDC = this->GetDlgItem(IDC_PIC)->GetDC();
	HDC hDispHdc = pDC->GetSafeHdc();
	pDC->GetWindow()->GetClientRect(rcDC);

	m_pMyDib->Paint(hDispHdc, rcDC, rcDIB); //�ؼ���344 

	/*��ǰߵ� */
	// ��������
	CPen *pPenRed = new CPen; 
	pPenRed->CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); //���û��ʲ���
	CGdiObject* pOldPen = pDC->SelectObject(pPenRed);
	pDC->SelectStockObject(NULL_BRUSH); // ����Բ������Ϊʵ��Բ
	for(int i = 0; i < keypoints.size(); i++)
	{
		pDC->Ellipse( keypoints[i].pt.x - keypoints[i].size,  
			nHeight/2 - keypoints[i].pt.y - keypoints[i].size, 
			          keypoints[i].pt.x + keypoints[i].size,
			nHeight/2 - keypoints[i].pt.y + keypoints[i].size ); //��ͼ�����ͼ�������Ǵ�ֱ���� 344 318
	}

	/// �ָ���ǰ�Ļ��� 
	delete pPenRed;
	pDC->SelectObject(pOldPen); 
	// ================================��ӻ�԰�������
	ReleaseDC(pDC);
}

//���ر����ļ�
void CDlgSortSystemOperater::OnBnClickedBtnLoadimage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pMyDib)
	{
		delete m_pMyDib;
        m_pMyDib=NULL;

		m_lWidth = 0;
		m_lHeight = 0;
	}
	m_pMyDib=new CMyDib();

	static char BASED_CODE file[] = ("BMP Files (*.bmp) |*.bmp|all type (*.*)|*.*||");//������
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
	
	//Ч�ʲ���1
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;

	
	//Ч�ʲ���2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	m_pMyDib->ReadBMPFile(m_strPath);

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();
	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);

	//Ч�ʲ���3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	char str[100] = {0};
	sprintf_s(str, sizeof(str),"ͼ��Ԥ�����ʱ%f ��", time);
	InfoDisplay(str);


	//Ч�ʲ���2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);

	ImageDetect();

	//Ч�ʲ���3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	//str[100] = {0};
	memset(str, 0, sizeof(str));
	sprintf_s(str, sizeof(str),"ͼ�����ʱ%f ��", time);
	InfoDisplay(str);

	//Ч�ʲ���2
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);

	DrawImage(m_lWidth,m_lHeight);

	//Ч�ʲ���3
	QueryPerformanceCounter(&nEndTime);
	time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	char str1[100] = {0};
	sprintf_s(str1, sizeof(str1),"��ʾͼ���ʱ%f ��", time);
	InfoDisplay(str1);
}

//
LRESULT CDlgSortSystemOperater::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
		switch(message)
	{
		  case WM_DISPLAYMESSAGE:
              EnterCriticalSection(&m_csShow);
			  //ͼ����
			  ImageDetect();
			  //ͼ����ʾ
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

//��֡�ɼ�
void CDlgSortSystemOperater::OnBnClickedBtnSnap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(true);

	//PLC ���M0.4
	int result = -1;
	if(!m_Opc->IsOPCOnline()) //OPC �����Լ��
	{
		m_Opc->InitOPCServer();
	}	
	while(1)
	{
		if(m_Opc->CheckPLCStateM0(4) != 1)  //����λ����
		{
			if(m_Opc->CheckPLCStateM0(2) == 1) //����λ����
			{
				InfoDisplay("����λ����"); //PLC ���洰��ʾM0.2
			}
			else
			{
				result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0 ����
				if(result == FALSE)
					continue;
			}
		}
		else
		{
			memset(pValues, 0, sizeof(OPCITEMSTATE));
			result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[3], OPC_DS_CACHE);  //VB1 ����λ
			if( pValues->vDataValue.bVal  == 0XFF)
				break;
		}
	}

	if (m_Acq == NULL && !m_Acq->IsHCPOnline())
	{
		InfoDisplay("ƽ��̽���������жϣ������������ӡ�");
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
				AfxMessageBox("ƽ��̽������ʼ��ʧ�ܣ�");
				InfoDisplay("ƽ��̽������ʼ��ʧ�ܣ�");
				m_Acq->Destroy(); 
				delete m_Acq;
				m_Acq = NULL;
				return ;
			}
		}
	}
	
	if (m_Acq->IsGrabbing())
	{
		m_Acq->Freeze(); //��ֹ�����ɼ�
	}

	if(!m_Acq->SnapImage())
	{
	    AfxMessageBox("�ɼ�ͼ��ʧ��!");	
	}

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();

	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
	ImageDetect();
}

//�Զ����
void CDlgSortSystemOperater::OnBnClickedBtnAutodetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_BTN_AUTODETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_MANUALDETECT)->EnableWindow(true);
	
	int result = -1;
	OPCITEMSTATE *pValues = new OPCITEMSTATE[1];
	
	while(1)
	{
		while(1) //�������
		{
			memset(pValues, 0, sizeof(OPCITEMSTATE));
			result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[6], OPC_DS_CACHE);  //VB4
			if( pValues->vDataValue.bVal  == 0XFF)
				break;
			Sleep(100);
		}

		while(1) 
		{
			if(m_Opc->CheckPLCStateM0(4) != 1)  //����λ����
			{
				if(m_Opc->CheckPLCStateM0(2) == 1) //����λ����
				{
					InfoDisplay("����λ����"); //PLC ���洰��ʾM0.2
				}
				else
				{
					result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0 ����
					if(result == FALSE)
						continue;
				}
			}
			else
			{
				memset(pValues, 0, sizeof(OPCITEMSTATE));
				result = m_Opc->ReadOPCServe(&pValues, &m_Opc->pdwServerHandles[3], OPC_DS_CACHE);  //VB1 ����λ
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
			AfxMessageBox("�ɼ�ͼ��ʧ��");
		}
		m_lWidth = m_pMyDib->Width();
		m_lHeight = m_pMyDib->Height();

		RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
		ImageDetect();

	}
	delete[] pValues;
}

//ͼ��ʶ��
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
		//�����ʾ
		InfoDisplay("���ʧ�ܣ�");
	}
	else if(ret == 0) //�����쳣
	{

	}
	else if(ret == 1) //ȼ���� �ϸ��
	{
		InfoDisplay("�ϸ�ȼ����");
		if(m_Opc != NULL && m_Opc->IsOPCOnline())
	    {
			result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[4], 0XFF);  //VB2
			if(result == FALSE)
			{
				return -1;
			}
	    }
				
	}
	else if(ret == 2) //ȼ���� ���ϸ�
	{
		keypoints = m_ImgProcess->GetKeypoints();
		InfoDisplay("���ϸ�ȼ����");
		if(m_Opc != NULL && m_Opc->IsOPCOnline())
		{
			result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[5], 0XFF);  //VB3
			if(result == FALSE)
			{
				return -1;
			}
		}
	}
	else if(ret == 3) //�����
	{
		InfoDisplay("�����");
	}
	else if(ret = 4) //����
	{
		InfoDisplay("����");
	}

	return 0;
}

//��ʱ�� ���plc״̬
void CDlgSortSystemOperater::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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


//�ֶ���� �л���ť
void CDlgSortSystemOperater::OnBnClickedBtnManualdetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
//���� 1 //����ִ�� ��4��
void CDlgSortSystemOperater::OnBnClickedBtnBallin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[2], 0XFF);  //VB0
	if(result == FALSE)
	{
		return ;
	}

}
//ʶ�� 2
void CDlgSortSystemOperater::OnBnClickedBtnDetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	m_lWidth = m_pMyDib->Width();
	m_lHeight = m_pMyDib->Height();

	RawDataToIplImage(m_pMyDib->GetImgDataAddr(), m_lWidth, m_lHeight);
	ImageDetect();
}
//ת�� 3
void CDlgSortSystemOperater::OnBnClickedBtnRoll()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
//���� 4
void CDlgSortSystemOperater::OnBnClickedBtnBallout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}



//��λ
void CDlgSortSystemOperater::OnBnClickedBtnReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxMessageBox("ϵͳ�쳣��ʹ�ø�λ������");
	int result = m_Opc->WriteOPCServe(&m_Opc->pdwServerHandles[7], 0XFF);  //VB5
	if(result == FALSE)
	{
		return ;
	}
}

//listbox ��ʾ��ʾ��Ϣ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlgSortSystemSQL CDlgParam;
	CDlgParam.m_pOwerFrm = reinterpret_cast<void *>(this);
	ShowWindow(SW_HIDE);
	CDlgParam.DoModal();
}
