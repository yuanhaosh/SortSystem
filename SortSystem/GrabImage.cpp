
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
	m_pRaw = NULL;           //Raw ��

	m_bTerminate = true;
	m_bHCPOnline = false;           //HCP����flag
	m_bHCPThreadIsActive = false;   //HCP�̴߳���
	m_bSendCallbackEnable = true;   //����ص�����

    InitializeCriticalSection(&m_criticalSection); 	// �ٽ���
}

CGrabImage::~CGrabImage(void)
{
	TerminateGrabThread();
	vip_close_link(0);//�Ͽ�HCP 0λ��ʼ����̽������-1λ����̽����
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

// ���ûص���������
void CGrabImage::SetOwner(void* pOwner, XRayGrabbedCallback funcCallback)
{
	m_pMainFrm = pOwner;
	m_funcCallback = funcCallback;
}

// ���������ų�ʼ�����
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

	if(m_bHCPOnline == false) //1 HCP �Ƿ����ӣ�û��������
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
		    m_bHCPOnline = true;  		//HCP���ӳɹ�
		}
		else
		{
			return FALSE;
		}
	}

	if(m_bHCPThreadIsActive == false && m_bHCPOnline)  //2 HCP�ɼ����߳��Ƿ��Ѿ�����
	{
		if (!CreateGrabThread())
		{
			return FALSE;
		}
	
		m_bHCPThreadIsActive = true;  //HCP�ɼ����߳��Ѿ�����
	}
	return TRUE;
}

// �����ɼ��߳�
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
	
	//�����ɼ��߳�
   	m_pWinThread = AfxBeginThread(GrabThread,this,0,0,0,NULL); 
	
    if (NULL == m_pWinThread) return false;	
	
	return TRUE;
}

// ��ֹ�ɼ��߳�
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

	m_bTerminate = true; //���߳��Զ�������free�߳��е��ڴ�
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

// HCP�ɼ�
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

	//��������ѭ���������˳���ʹ���¼������������ɼ��͵��βɼ�
	for( ; ; )
	{
		WaitForSingleObject(pThreadOwner->m_hEventToRun, INFINITE);	// ���յ������вɼ�ѭ�����¼���������֡�ɼ��������ɼ�
		if (pThreadOwner->m_bTerminate)								// ��������̱߳�־Ϊ�棬���˳�����ѭ��
		{
			break;
		}
		switch(pThreadOwner->m_HCPWorkMode)
		{ 
			case eXRaySnap: //��֡�ɼ� ��ʱû��
				{
					status = pThreadOwner->AcquireImage();		// �ɼ���ȡһ��ͼ��

					if (pThreadOwner->m_bSendCallbackEnable)
					{
						//ƽ��̽������������һ���ʽ��һ�£��ú���ת����Ҫ��m_RawPtr to m_pGrabImage
						int ret = pThreadOwner->TransfRawToByte();
						if(ret == -1)
						{
							return -1;
						}
						pThreadOwner->GrabbedImageSendCallbackToDisplay(status);	// ����GrabbedImageSendCallbackToDisplay(true)����
					}
				
					ResetEvent(pThreadOwner->m_hEventToRun);				// ����������вɼ�
					SetEvent(pThreadOwner->m_hEventSnapped);				// ֪ͨ�����߳�ֹͣ�ȴ�
				}
				break;
			case eXRayGrab: //�����ɼ�
				{
					pThreadOwner->m_bHaveStopped = !pThreadOwner->m_bIsGrabbing;
				
					while (pThreadOwner->m_bIsGrabbing)					//���������ɼ�
					{
						status = pThreadOwner->AcquireImage();		// �ɼ���ȡһ��ͼ��

						if (pThreadOwner->m_bSendCallbackEnable)
						{
							//ƽ��̽������������һ���ʽ��һ�£��ú���ת����Ҫ��m_RawPtr to m_pGrabImage
							int ret = pThreadOwner->TransfRawToByte();
							if(ret == -1)
							{
								return -1;
							}
							pThreadOwner->GrabbedImageSendCallbackToDisplay(status);			// ����GrabbedImageSendCallbackToDisplay(status)����
						}		
					}
				
					pThreadOwner->m_bHaveStopped = true;					// ��������ɼ��Ѿ�ֹͣ��
					ResetEvent(pThreadOwner->m_hEventToRun);				// ����������вɼ�
				
				}break;

			default: break; //eXRayIdle

		}
	}
	
	SetEvent(pThreadOwner->m_hEventExit);	//�����˳��ź�
	return 0;
}

//��ע:һ���ǵ��ͷ��ٽ��� 
//���grab��ֱ��ץȡͼƬ���������߳� �����汾��ͬ���� 1
XRayStatus CGrabImage::AcquireImage()
{
    EnterCriticalSection(&m_criticalSection); //�����ٽ���
	 

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
		result = vip_fluoro_record_start(); //ֱ�ӵ��ø���
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
			result = CHcp->get_mode_info(); //��ȡrawͼ����
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

	//��ȡͼ�񣬸��ݷ���ֵ�����ж�
	if(result == VIP_NO_ERR)
	{
		m_lWidth = CHcp->get_imx();
		m_lHeight = CHcp->get_imy();
		result = vip_get_image(Mode, VIP_CURRENT_IMAGE, m_lWidth, m_lHeight, m_RawPtr);
	}

	LeaveCriticalSection(&m_criticalSection); //�ͷ��ٽ���1

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
			return -1; //LOG ����8λͼƬ�洢���쳣
		}
	}
	else
	{
		memset(m_pGrabImage, 0,  totalpiexls * sizeof(unsigned char));
	}

	if(m_pRaw == NULL)
	{
		m_pRaw = new CRaw(m_RawPtr,  m_pGrabImage, 12, m_lWidth, m_lHeight); //�����ļ�����
	}
	(m_pRaw)->RangeConversionToEightBit();
	return 0;
}

void CGrabImage::GrabbedImageSendCallbackToDisplay(XRayStatus status)
{
	// ���ûص�����
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

//�����ɼ����
bool CGrabImage::Grab()
{
	if (!m_bHCPOnline )
	{
		return false; //LOG HCPδ����
	}
	

	m_HCPWorkMode = eXRayGrab; 
	m_bIsGrabbing = true;
	SetEvent(m_hEventToRun);
	
	return true;
}	

// ���������ɼ�	
bool CGrabImage::Freeze() 
{
	if (!m_bHCPOnline)
	{
		return false; //LOG HCPδ����
	}

	// �̲߳ɼ���ʽ
	if (m_bIsGrabbing || !m_bHaveStopped)
	{
		m_HCPWorkMode = eXRayIdle;
		m_bIsGrabbing = false;
		SetEvent(m_hEventToRun);  //��ѭ���õ�������
		int i = 0;
		while (!m_bHaveStopped && i < 500)
		{
			Sleep(2);
			i++;	
		}
	}
	return true;
}

// �ɼ���֡ͼ��
// �òɼ��������������ɼ�����ͬʱ���ã�
// �òɼ�����ֱ���ɼ���ɺ��˳���
bool CGrabImage::SnapImage()
{
	XRayStatus status;

	if (!m_bHCPOnline )
	{
		//��������ֱ���˳����ַ�ʽ
		return false; //LOG HCPδ����
	}
	m_lWidth = 0L;
	m_lHeight = 0L;
	m_bAcquireSuceess = false; 
	status = AcquireImage();		// �ɼ���ȡһ��ͼ��
	m_bAcquireSuceess = true;
	if (m_bSendCallbackEnable)
	{
		int ret = TransfRawToByte();
		if(ret == -1)
		{
			return false;
		}
		GrabbedImageSendCallbackToDisplay(status);			// ����GrabbedImageSendCallbackToDisplay(status)����
	}		
	
	return m_bAcquireSuceess;
}

// ͼ���Ƿ�ɼ��ɹ� not used
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

//�ͷ����п��ٵ��ڴ�
BOOL CGrabImage::Destroy()
{
	TerminateGrabThread();

	if (m_pGrabImage != NULL) // ���
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