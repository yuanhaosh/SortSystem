//*******************************************************************
//              
//  FILE:       HcpAction.cpp
//              
//  AUTHOR:     袁浩
//              
//  PROJECT:    
//              
//  COMPONENT:  
//              
//  DATE:        2014/04/26
//              
//  COMMENTS:    VirtCp.dll中函数组成的类，方便调用扩展使用
//                          
//*******************************************************************

#include "stdafx.h"
#include "HcpAction.h"

//UQueryProgInfo    cHcpAction::prevStatus;

cHcpAction::cHcpAction()
{
	m_GrabbingIsActive = false;
	m_ImgDisplayThreadIsAlive = 0;
	m_NumFrames = 0;
	m_iRealFrame = 0;
	m_imX = 0;
	m_imY = 0;
}

cHcpAction::~cHcpAction()
{
	
}

int cHcpAction::open_receptor_links(char HcpRecdirPath[MAX_STR])
{
	
	struct  SOpenReceptorLink orl;
	memset(&orl, 0, sizeof(SOpenReceptorLink));
	orl.StructSize = sizeof(SOpenReceptorLink);
	strncpy(orl.RecDirPath, HcpRecdirPath, MAX_STR);
	orl.DebugMode = 0;
	
	result = vip_open_receptor_link(&orl);
	return result;
}

//这些校正最好全开
int cHcpAction::set_correction_settings(int iOfst, int iGain,int iDfct,int iRotate90, int iFlipX,int iFlipY)
{
	memset(&corr, 0, sizeof(SCorrections));
	corr.StructSize = sizeof(SCorrections);
	
	// here we will just set corrections as needed for test purposes
	// but IN REAL APPLICATIONS we must be sure basic offset, gain and
	// defect CORRECTIONS ARE ON and the receptor is CALIBRATED
	corr.Ofst = Ofst = iOfst;
	corr.Gain = Gain = iGain;
	corr.Dfct = Dfct = iDfct;
	
	// additional image manipulation operations may be set optionally
	/*这三个参数对应将图像旋转 镜像处理 暂时用不到*/
	corr.Rotate90 = Rotate90 = iRotate90;
	corr.FlipX    = FlipX    = iFlipX;
	corr.FlipY    = FlipY    = iFlipY;
	
	result = vip_set_correction_settings(&corr);
	return result;
}

int cHcpAction::get_sys_info()
{
	
	memset(&sdi, 0, sizeof(SDeviceInfo1));
	sdi.StructSize = sizeof(SDeviceInfo1);
	sdi.StructType = 1;
	
	memset(&sysInfo, 0, sizeof(SSysInfo));
	sysInfo.StructSize = sizeof(SSysInfo);
	sysInfo.DeviceInfoPtr = &sdi;
	
	result = vip_get_sys_info(&sysInfo);
	
	if(result == HCP_NO_ERR)
	{
		NumModes = sysInfo.NumModes;
	}
	else
	{	
		//"Get system info error."
	}
	
	return result;
	
}

int cHcpAction::select_get_mode( int mode)
{	
	result = vip_select_mode(mode);
	
	//if (result == HCP_NO_ERR   || result == HCP_OFST_ERR ||
	//	result == HCP_GAIN_ERR || result == HCP_DFCT_ERR)
	//{
	//	//result = SetCorrections(result);
	//	memset(&corr, 0, sizeof(SCorrections));
	//	corr.StructSize = sizeof(SCorrections);
	//	corr.Ofst = Ofst;
	//	corr.Gain = Gain;
	//	corr.Dfct = Dfct;
	//	
	//	// additional image manipulation operations may be set optionally
	//	corr.Rotate90 = Rotate90;
	//	corr.FlipX    = FlipX;
	//	corr.FlipY    = FlipY;
	//	
	//	result = vip_set_correction_settings(&corr);
	//}
	//
	//// check mode
	//if(result == HCP_NO_ERR)
	//{
	//	memset(&mdInf, 0, sizeof(SModeInfo));
	//	mdInf.StructSize = sizeof(SModeInfo);
	//	
	//	result = vip_get_mode_info(mdIdx, &mdInf);
	//	
	//	if(result == HCP_NO_ERR)
	//	{
	//		int imX = mdInf.ColsPerFrame;
	//		int imY = mdInf.LinesPerFrame; //这两个值得大小与模式有关系分别为521和1024
	//		
	//		if((mdInf.AcqType & VIP_ACQ_MASK) != VIP_ACQ_TYPE_CONTINUOUS)
	//		{
	//			result = HCP_OTHER_ERR;
	//			// "Selected mode is not a standard FLUORO mode."
	//		}
	//		else
	//		{
	//			CbDataType = mdInf.AcqType & VIP_CB_MASK;
	//		}
	//	}
	//	/*	if(result == HCP_NO_ERR)
	//	{
	//	switch(GCbDataType)
	//	{
	//	case VIP_CB_NORMAL:
	//	//printf("\n\nNORMAL mode type selected\n");
	//	memset(retStr, 0, sizeof(retStr));
	//	sprintf(retStr, "NORMAL mode type selected.");
	//	write_log(GFdLog, 'N', retStr);
	//	break;
	//	case VIP_CB_DUAL_READ:
	//	//printf("\n\nDUAL READ mode type selected\n");
	//	memset(retStr, 0, sizeof(retStr));
	//	sprintf(retStr, "DUAL READ mode type selected.");
	//	write_log(GFdLog, 'N', retStr);
	//	break;
	//	case VIP_CB_DYNAMIC_GAIN:
	//	//printf("\n\nDYNAMIC GAIN SWITCHING mode type selected\n");
	//	memset(retStr, 0, sizeof(retStr));
	//	sprintf(retStr, "DYNAMIC GAIN SWITCHING mode type selected.");
	//	write_log(GFdLog, 'N', retStr);
	//	break;
	//	default:
	//	result = HCP_LOGIC_ERR;
	//	}
	//}*/
	//}
	return result;
}


int cHcpAction::checkmode(int mdIdx)
{
	SModeInfo mdInf;
	if(result == HCP_NO_ERR)
	{
		memset(&mdInf, 0, sizeof(SModeInfo));
		mdInf.StructSize = sizeof(SModeInfo);
		result = vip_get_mode_info(mdIdx, &mdInf);
		
	}

	if(result == HCP_NO_ERR)
	{
		m_imX = mdInf.ColsPerFrame;
		m_imY = mdInf.LinesPerFrame;
	
		ImgSize = m_imX * m_imY * sizeof(WORD); // image size in bytes  

		if((mdInf.AcqType & VIP_ACQ_MASK) != VIP_ACQ_TYPE_CONTINUOUS)
		{
			result = HCP_OTHER_ERR;
		}	
	}
	return result;
}

int cHcpAction::fluoro_set_prms(int buf, int frm)
{
	
	memset(&seqPrms, 0, sizeof(SSeqPrms));
	seqPrms.StructSize = sizeof(SSeqPrms);
	seqPrms.NumBuffers = buf;
	seqPrms.StopAfterN = frm; // zero is interpreted as acquire continuous(writing to buffers in circular fashion)
	
	result = vip_fluoro_set_prms(HCP_FLU_SEQ_PRMS, &seqPrms);
	buf = seqPrms.NumBuffers; //内存可能不够分配，所以buf可能会小于frm
	if(frm > buf) 
	{
		m_iRealFrame = buf;
	}
	return result;
}

int cHcpAction::get_mode_info()
{
	SModeInfo mdInf;
	memset(&mdInf, 0, sizeof(SModeInfo));
	mdInf.StructSize = sizeof(SModeInfo);
	//int result = vip_get_mode_info(GSelectedMode, &mdInf);
	if(result == HCP_NO_ERR)
	{
		m_imX = mdInf.ColsPerFrame;
		m_imY = mdInf.LinesPerFrame;
	}
	return result;
}

int cHcpAction::get_pixels()const
{
	return (m_imX * m_imY);
}

int cHcpAction::get_imx()
{
return m_imX;
}

int cHcpAction::get_imy()
{
return m_imY;
}
int cHcpAction::get_allocate_real_Frame()
{
	return m_iRealFrame;
}

long cHcpAction::get_thread_frame()
{
	return m_NumFrames;
}

bool cHcpAction::get_grab_state()
{
	return m_GrabbingIsActive;
}
void cHcpAction::set_grab_state(bool state)
{
	m_GrabbingIsActive = state;
}

int cHcpAction::get_correction_settings()
{
	memset(&corr, 0, sizeof(SCorrections));
	corr.StructSize = sizeof(SCorrections);
	
	result = vip_get_correction_settings(&corr);
	return result;
}

long  cHcpAction::get_display_thread_state()
{
	return m_ImgDisplayThreadIsAlive;
}

int cHcpAction::fluoro_grabber_start( void *ptr, int corrtype = HCP_CORR_STD)
{
	

	memset(&acqPrms, 0, sizeof(SAcqPrms));
	acqPrms.StructSize = sizeof(SAcqPrms);
	
    acqPrms.CorrType = corrtype;  //参看demo此处的注释
	acqPrms.CorrFuncPtr = ptr; 
	acqPrms.ReqType = 0;   // ReqType is for internal use only. Must be left at zero always.
	
	result = vip_fluoro_grabber_start(&acqPrms);

	m_LiveParams = (SLivePrms*)acqPrms.LivePrmsPtr;
	return 0;
}

int cHcpAction::fluoro_get_event_name(int EventType, char name[MAX_STR])
{
    //memcpy(FrameReadyName, name, sizeof(name));
   // result = vip_fluoro_get_event_name(num, FrameReadyName); //add 0806 多余的步骤
	result = vip_fluoro_get_event_name(EventType, name); 
	return result;
}

int cHcpAction::fluoro_record_start(int numFrm = 0, int startFromBufIdx = -1)
{
	result = vip_fluoro_record_start(numFrm, startFromBufIdx);
	return result;
}

int cHcpAction::fluoro_record_stop()
{
	result = vip_fluoro_record_stop();
	return result;
}

int cHcpAction::fluoro_grabber_stop()
{
	result = vip_fluoro_grabber_stop();
	return result;
}

int cHcpAction::fluoro_get_buffer_ptr(WORD** buf, int bufIdx)
{
	result = vip_fluoro_get_buffer_ptr(buf, bufIdx);
	return result;
}

int cHcpAction::reset_state()
{
	result = vip_reset_state();
	return result;
}

int cHcpAction::offset_cal(int SelectedMode)
{
	result = vip_offset_cal(SelectedMode);
	return result;
}

int cHcpAction::query_prog_info(int uType, UQueryProgInfo* uq)
{
	result = vip_query_prog_info(uType, uq);
	//_snprintf(retStr, MAX_STR, "vip_query_prog_info()");
	return result;
}

int cHcpAction::get_num_cal_frames(int mode_num, int* num_cal_frames)
{
	result = vip_get_num_cal_frames(mode_num, num_cal_frames);
	return result;
}

int cHcpAction::gain_cal_prepare(int mode_num)
{
	result = vip_gain_cal_prepare(mode_num);
	return result;
}

int cHcpAction::sw_handshaking(int signal_type, BOOL active)
{
	result = vip_sw_handshaking(signal_type, active);
	return result;
}