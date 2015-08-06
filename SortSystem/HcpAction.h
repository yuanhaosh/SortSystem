//*******************************************************************
//              
//  FILE:       HcpAction.h
//
//  AUTHOR:     袁浩
//
//  PROJECT:    
//
//  COMPONENT:  
//
//  DATE:        2014/04/26
//
//  COMMENTS:    VirtCp.dll中函数组成的类，方便调用扩展使用.同时包含一些平板中使用的常用变量
//
//*******************************************************************

#include "HcpErrors.h"
#include "HcpFuncDefs.h"

class cHcpAction
{
public:
	cHcpAction();
	~cHcpAction();
	
	int  NumModes;                   //指示平板有几种模式可以选择
//	bool LinkIsOpen;                 //连接平板标志位
	bool m_GrabbingIsActive;           //启动grab正常标志位
    long ImgDisplayThreadIsAlive;    //图像抓取线程运行标志位
	int  m_imX;
	int  m_imY;
	int  ImgSize;
	long m_ImgDisplayThreadIsAlive;    //锁定线程
	long m_NumFrames;                  //线程中用于统计图像帧数
	int  m_iRealFrame;               //平板探测器实际分配的帧数

	
	struct  SOpenReceptorLink orl;
    struct  SCorrections      corr;
	struct  SDeviceInfo1      sdi;
	struct  SSysInfo          sysInfo;
	struct	SModeInfo         mdInf;
	struct  SSeqPrms          seqPrms;
	struct  SLivePrms*	      m_LiveParams; //used
	struct  SAcqPrms          acqPrms;      //used
	UQueryProgInfo            CrntStatus;
 //   static  UQueryProgInfo    prevStatus; //源代码是静态的static  
//	static bool   prevInit;

	//char    FrameReadyName[MAX_STR];
	
	int open_receptor_links(char HcpRecdirPath[MAX_STR]);
	
	int set_correction_settings(int iOfst, int iGain,int iDfct,int iRotate90, int iFlipX,int iFlipY);
	
	int checkmode(int mdIdx);

	int get_mode_info();

	int get_sys_info();

	int select_get_mode(int index);

	int fluoro_set_prms(int buf, int frm);
	
	int  get_allocate_real_Frame();
	long get_thread_frame();
	long get_display_thread_state();
	bool get_grab_state();
	void set_grab_state(bool state);
    int get_pixels() const;
	int get_imx();
	int get_imy();

	int get_correction_settings();

	int fluoro_grabber_start( void *ptr, int corrtype);

	int fluoro_get_event_name(int EventType, char name[MAX_STR]);

	int fluoro_record_start(int numFrm, int startFromBufIdx);

	int fluoro_record_stop();

	int fluoro_grabber_stop();

	int fluoro_get_buffer_ptr(WORD** buf, int bufIdx);

	int reset_state();

	int offset_cal(int SelectedMode);

	int query_prog_info(int uType, UQueryProgInfo* uq);

	int get_num_cal_frames(int mode_num, int* num_cal_frames);

	int gain_cal_prepare(int mode_num);

	int sw_handshaking(int signal_type, BOOL active);

	
    // a thread used to display or process images in real time
  // static DWORD /*WINAPI*/ ImgDisplayThread(LPVOID lpParameter);
	
protected:
	int result;

	int  Ofst;       //校正方式
	int  Gain;  
	int  Dfct;  
	
	int  Rotate90; 
	int  FlipX; 
	int	 FlipY; 

	int  CbDataType; //检测模式信息
		
};