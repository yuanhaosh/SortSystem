
#include "stdafx.h"
#include "GrabImage.h"
#include <windowsx.h>

CGrabImage::CGrabImage(void)
{
	CHcp = NULL;
	result = HCP_NO_ERR;
    memset(FrameReadyName, 0, sizeof(FrameReadyName));
	m_RawPtr = NULL;         //store raw data
	m_pGrabImage =	NULL;    //store 8bit image data
	m_pRaw = NULL;           //Raw 类

	m_bTerminate = true;
	m_bHCPOnline = false;           //HCP连接flag
	m_bHCPThreadIsActive = false;   //HCP线程待命
	m_bSendCallbackEnable = true;   //允许回调函数

    InitializeCriticalSection(&m_criticalSection); 	// 临界区
}

CGrabImage::~CGrabImage(void)
{
	TerminateGrabThread();
	vip_close_link(0);//断开HCP 0位起始索引探测器，-1位所有探测器
	m_bHCPOnline = false;

	if (m_pGrabImage != NULL)
	{
		GlobalFreePtr(m_pGrabImage);
		m_pGrabImage = NULL;
	}

	if(m_RawPtr != NULL)
	{
		GlobalFreePtr(m_RawPtr);
		m_RawPtr = NULL;
	}

	m_lWidth = 0;
	m_lHeight = 0;
	DeleteCriticalSection(&m_criticalSection); 

}

// 设置回调函数参数
void CGrabImage::SetOwner(void* pOwner, XRayGrabbedCallback funcCallback)
{
	m_pMainFrm = pOwner;
	m_funcCallback = funcCallback;
}

// 根据索引号初始化相机
BOOL CGrabImage::InitHCPFromIndex(sHcpParam* sHcpParamter) 
{
	if(CHcp != NULL)
	{
		delete CHcp;
		CHcp = NULL;
	}
	else
	{
		CHcp = new cHcpAction();
	}
	
	Buf = sHcpParamter->Buf;
	Frm = sHcpParamter->Frm;
	Mode = sHcpParamter->Mode;
	 
	return InitHCP(sHcpParamter);
}

BOOL CGrabImage::InitHCP(sHcpParam* sHcpParamter)
{

	if(m_bHCPOnline == false) //1 HCP 是否连接，没有则连接
	{
		result = CHcp->open_receptor_links( sHcpParamter->HcpPath );
		if (result == HCP_NO_ERR   || result == HCP_OFST_ERR ||
			result == HCP_GAIN_ERR || result == HCP_DFCT_ERR)
		{
			result = CHcp->select_get_mode(sHcpParamter->Mode);
		}
		if(result == HCP_NO_ERR   || result == HCP_OFST_ERR ||
			result == HCP_GAIN_ERR || result == HCP_DFCT_ERR)
		{
			result = CHcp->set_correction_settings(sHcpParamter->OffsetCorr,
													sHcpParamter->GainCorr, 
													sHcpParamter->DftCorr, 
														0, 0, 0);
		}

		if(result == HCP_NO_ERR)
		{
			result = CHcp->checkmode(Mode);
		}

		if(result == HCP_NO_ERR)
		{
		    m_bHCPOnline = true;  		//HCP连接成功
		}
		else
		{
			return FALSE;
		}
	}

	if(m_bHCPThreadIsActive == false && m_bHCPOnline)  //2 HCP采集的线程是否已经待命
	{
		if (!CreateGrabThread())
		{
			return FALSE;
		}
	
		m_bHCPThreadIsActive = true;  //HCP采集的线程已经待命
	}
	return TRUE;
}

// 创建采集线程
BOOL CGrabImage::CreateGrabThread()
{	
	if (!m_bTerminate)
	{
		return false;
	}
	
	m_bTerminate = false;
	m_hEventSnapped = CreateEvent(NULL,true,false,_T(""));
	m_hEventToRun = CreateEvent(NULL,true,false,_T(""));
	m_hEventExit = CreateEvent(NULL,true,false,_T(""));
	
	//创建采集线程
   	m_pWinThread = AfxBeginThread(GrabThread,this,0,0,0,NULL); 
	
    if (NULL == m_pWinThread) return false;	
	
	return TRUE;
}

// 终止采集线程
BOOL CGrabImage::TerminateGrabThread()
{
	if (!m_bHCPOnline)
	{
		return false;
	}
	if (m_bTerminate)
	{
		return true;
	}

	m_bTerminate = true; //让线程自动结束后free线程中的内存
	if (WAIT_TIMEOUT == WaitForSingleObject(m_hEventExit, 1000))
	{
		Sleep(500);
	}	
	if (m_pWinThread != NULL)
	{
		m_pWinThread = NULL;
	}	

	if(m_pRaw != NULL)
	{
		delete m_pRaw;
		m_pRaw = NULL;
	}

	CloseHandle(m_hEventExit);
	m_hEventExit = NULL;

	CloseHandle(m_hEventSnapped);
	m_hEventSnapped = NULL;

	CloseHandle(m_hEventToRun);
	m_hEventToRun = NULL;

	return TRUE;
}

// HCP采集
UINT CGrabImage::GrabThread(LPVOID lpParam)
{ 
	if (lpParam == NULL)
	{
		return -1;
	}
	int ret = 0;
	XRayStatus status;
	CGrabImage *pThreadOwner=(CGrabImage*)lpParam;
	
	if(pThreadOwner->m_bHCPThreadIsActive == false)
	{
		return -1;
	}

	//这里是死循环，不能退出。使用事件来控制连续采集和单次采集
	for( ; ; )
	{
		WaitForSingleObject(pThreadOwner->m_hEventToRun, INFINITE);	// 当收到了运行采集循环的事件，包括单帧采集和连续采集
		if (pThreadOwner->m_bTerminate)								// 如果结束线程标志为真，则退出总体循环
		{
			break;
		}
		switch(pThreadOwner->m_HCPWorkMode)
		{ 
			case eXRaySnap: //单帧采集 暂时没用
				{
					status = pThreadOwner->AcquireImage();		// 采集获取一幅图像

					if (pThreadOwner->m_bSendCallbackEnable)
					{
						//平板探测器的数据与一般格式不一致，用函数转成需要的m_RawPtr to m_pGrabImage
						int ret = pThreadOwner->TransfRawToByte();
						if(ret == -1)
						{
							return -1;
						}
						pThreadOwner->GrabbedImageSendCallbackToDisplay(status);	// 调用GrabbedImageSendCallbackToDisplay(true)函数
					}
				
					ResetEvent(pThreadOwner->m_hEventToRun);				// 允许继续进行采集
					SetEvent(pThreadOwner->m_hEventSnapped);				// 通知调用线程停止等待
				}
				break;
			case eXRayGrab: //连续采集
				{
					pThreadOwner->m_bHaveStopped = !pThreadOwner->m_bIsGrabbing;
				
					while (pThreadOwner->m_bIsGrabbing)					//允许连续采集
					{
						status = pThreadOwner->AcquireImage();		// 采集获取一幅图像

						if (pThreadOwner->m_bSendCallbackEnable)
						{
							//平板探测器的数据与一般格式不一致，用函数转成需要的m_RawPtr to m_pGrabImage
							int ret = pThreadOwner->TransfRawToByte();
							if(ret == -1)
							{
								return -1;
							}
							pThreadOwner->GrabbedImageSendCallbackToDisplay(status);			// 调用GrabbedImageSendCallbackToDisplay(status)函数
						}		
					}
				
					pThreadOwner->m_bHaveStopped = true;					// 标记连续采集已经停止了
					ResetEvent(pThreadOwner->m_hEventToRun);				// 允许继续进行采集
				
				}break;

			default: break; //eXRayIdle

		}
	}
	
	SetEvent(pThreadOwner->m_hEventExit);	//发出退出信号
	return 0;
}

//备注:一定记得释放临界区 
//这个grab后直接抓取图片，不包含线程 两个版本不同流程 1
XRayStatus CGrabImage::AcquireImage()
{
    EnterCriticalSection(&m_criticalSection); //进入临界区
	 

	////////////////step 1 Set sequence info/////////////////
	result = CHcp->fluoro_set_prms(Buf, Frm);
	if(VIP_NO_ERR == result)
	{
	    Frm = CHcp->get_allocate_real_Frame();
		Buf = Frm;
	    result = CHcp->get_correction_settings();
	}

	//////////////step 2 Start the grabber/////////////////////
	if(result == HCP_NO_ERR)
	{
		CHcp->get_correction_settings();
		if(result == HCP_NO_ERR)
		{
			result = CHcp->fluoro_grabber_start(NULL, HCP_CORR_STD);
		}
	}
	//////////////////step 3 Start the recording/////////////////////
	if(result == VIP_NO_ERR)
	{
		fflush(stdin);
		result = vip_fluoro_record_start(); //直接调用更快
	}

	////////////////////step 4 Wait for end of recording//////////////////////////
	if(result == VIP_NO_ERR)
	{
		int to = 100 * 1000; //ms. RECORD_TIMEOUT = 100
		const int lpNum = 500;
		int qRate = to / lpNum + 1;
		for(int i=0; i<lpNum; i++)
		{
			if(CHcp->m_LiveParams && (CHcp->m_LiveParams)->NumFrames >= Frm) break;
			Sleep(qRate);
		}

		TRACE("\nFrames recorded = %d", (CHcp->m_LiveParams)->NumFrames);
	
		result = vip_fluoro_grabber_stop();
	}

	//////////////////////step 5  Save an image to file//////////////////////////////
	if(result == VIP_NO_ERR)
	{
		WORD* buf=NULL;
		int frmIdx=0;
		TRACE("\nPick a frame index to save? ");
		//scanf("%d",&frmIdx);
		frmIdx = 1; //which frame based on debug. yuanhao
		if(frmIdx >= 0 && frmIdx < Frm)
		{
			result = vip_fluoro_get_buffer_ptr(&buf, frmIdx);
		}

		if(result == VIP_NO_ERR)
		{
			//result = WriteImageToFile(buf, retStr);
			result = CHcp->get_mode_info(); //获取raw图像宽高
		}
	}

	if(result == VIP_NO_ERR)
	{

		if(m_RawPtr == NULL)
		{
			m_RawPtr = (WORD *)GlobalAllocPtr(GHND, CHcp->get_pixels() * sizeof(WORD));
		    result = (m_RawPtr != NULL) ? HCP_NO_ERR : HCP_MEMALLOC_ERR;
			//memset(m_RawPtr, 0, (CHcp->get_pixels() * sizeof(WORD)) );
		}
		else
		{
			memset(m_RawPtr, 0, (CHcp->get_pixels() * sizeof(WORD) ) );
		}
	}

	//获取图像，根据返回值作出判断
	if(result == VIP_NO_ERR)
	{
		m_lWidth = CHcp->get_imx();
		m_lHeight = CHcp->get_imy();
		result = vip_get_image(Mode, VIP_CURRENT_IMAGE, m_lWidth, m_lHeight, m_RawPtr);
	}

	LeaveCriticalSection(&m_criticalSection); //释放临界区1

	XRayStatus status;
	if(result == HCP_NO_ERR)
	    status = eXRayGrabOK;
	else
		status = eXRayGrabFailed;

	return status;
}

int CGrabImage::TransfRawToByte()
{
	int totalpiexls = m_lWidth * m_lHeight;
	if((m_pGrabImage) == NULL)
	{
		(m_pGrabImage) = (unsigned char* )GlobalAllocPtr(GHND, totalpiexls * sizeof(unsigned char));
		if(m_pGrabImage == NULL) 
		{
			return -1; //LOG 分配8位图片存储区异常
		}
	}
	else
	{
		memset(m_pGrabImage, 0,  totalpiexls * sizeof(unsigned char));
	}

	if(m_pRaw == NULL)
	{
		m_pRaw = new CRaw(m_RawPtr,  m_pGrabImage, 12, m_lWidth, m_lHeight); //配置文件参数
	}
	(m_pRaw)->RangeConversionToEightBit();
	return 0;
}

void CGrabImage::GrabbedImageSendCallbackToDisplay(XRayStatus status)
{
	// 调用回调函数
	if( (m_funcCallback != NULL) && (m_pGrabImage != NULL) )
	{
		if (eXRayGrabOK == status)
		{
			m_funcCallback(m_pMainFrm, status, m_pGrabImage, m_lWidth, m_lHeight, m_pixelFormat);		
		}
		else
		{
			m_funcCallback(m_pMainFrm, status, NULL, NULL, NULL, eXRayUnknown);		
		}
	}
}

//连续采集检测
bool CGrabImage::Grab()
{
	if (!m_bHCPOnline )
	{
		return false; //LOG HCP未连接
	}
	

	m_HCPWorkMode = eXRayGrab; 
	m_bIsGrabbing = true;
	SetEvent(m_hEventToRun);
	
	return true;
}	

// 结束连续采集	
bool CGrabImage::Freeze() 
{
	if (!m_bHCPOnline)
	{
		return false; //LOG HCP未连接
	}

	// 线程采集方式
	if (m_bIsGrabbing || !m_bHaveStopped)
	{
		m_HCPWorkMode = eXRayIdle;
		m_bIsGrabbing = false;
		SetEvent(m_hEventToRun);  //将循环置到空闲上
		int i = 0;
		while (!m_bHaveStopped && i < 500)
		{
			Sleep(2);
			i++;	
		}
	}
	return true;
}

// 采集单帧图像；
// 该采集函数不能与他采集函数同时调用；
// 该采集函数直到采集完成后退出；
bool CGrabImage::SnapImage()
{
	XRayStatus status;

	if (!m_bHCPOnline )
	{
		//重连或者直接退出两种方式
		return false; //LOG HCP未连接
	}
	m_lWidth = 0L;
	m_lHeight = 0L;
	m_bAcquireSuceess = false; 
	status = AcquireImage();		// 采集获取一幅图像
	m_bAcquireSuceess = true;
	if (m_bSendCallbackEnable)
	{
		int ret = TransfRawToByte();
		if(ret == -1)
		{
			return false;
		}
		GrabbedImageSendCallbackToDisplay(status);			// 调用GrabbedImageSendCallbackToDisplay(status)函数
	}		
	
	return m_bAcquireSuceess;
}

// 图像是否采集成功 not used
bool CGrabImage::IsAcquireSuccess()
{
	return m_bAcquireSuceess;
}

bool CGrabImage::IsGrabbing()
{
	return m_bIsGrabbing;
}
bool CGrabImage::IsHCPOnline()
{
	return m_bHCPOnline;
}

//释放类中开辟的内存
BOOL CGrabImage::Destroy()
{
	TerminateGrabThread();

	if (m_pGrabImage != NULL) // 清空
	{
		GlobalFreePtr(m_pGrabImage);
		m_pGrabImage = NULL;
	}
	if(m_RawPtr != NULL)
	{
		GlobalFreePtr(m_RawPtr);
		m_RawPtr = NULL;
	}

	return TRUE;
}