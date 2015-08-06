//*******************************************************************
//              
//  FILE:       HcpAction.h
//
//  AUTHOR:     Ԭ��
//
//  PROJECT:    
//
//  COMPONENT:  
//
//  DATE:        2014/04/26
//
//  COMMENTS:    VirtCp.dll�к�����ɵ��࣬���������չʹ��.ͬʱ����һЩƽ����ʹ�õĳ��ñ���
//
//*******************************************************************

#include "HcpErrors.h"
#include "HcpFuncDefs.h"

class cHcpAction
{
public:
	cHcpAction();
	~cHcpAction();
	
	int  NumModes;                   //ָʾƽ���м���ģʽ����ѡ��
//	bool LinkIsOpen;                 //����ƽ���־λ
	bool m_GrabbingIsActive;           //����grab������־λ
    long ImgDisplayThreadIsAlive;    //ͼ��ץȡ�߳����б�־λ
	int  m_imX;
	int  m_imY;
	int  ImgSize;
	long m_ImgDisplayThreadIsAlive;    //�����߳�
	long m_NumFrames;                  //�߳�������ͳ��ͼ��֡��
	int  m_iRealFrame;               //ƽ��̽����ʵ�ʷ����֡��

	
	struct  SOpenReceptorLink orl;
    struct  SCorrections      corr;
	struct  SDeviceInfo1      sdi;
	struct  SSysInfo          sysInfo;
	struct	SModeInfo         mdInf;
	struct  SSeqPrms          seqPrms;
	struct  SLivePrms*	      m_LiveParams; //used
	struct  SAcqPrms          acqPrms;      //used
	UQueryProgInfo            CrntStatus;
 //   static  UQueryProgInfo    prevStatus; //Դ�����Ǿ�̬��static  
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

	int  Ofst;       //У����ʽ
	int  Gain;  
	int  Dfct;  
	
	int  Rotate90; 
	int  FlipX; 
	int	 FlipY; 

	int  CbDataType; //���ģʽ��Ϣ
		
};